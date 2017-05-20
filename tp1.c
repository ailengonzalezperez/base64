//#include "base64.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

char letters_table[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

void show_help(){
		printf("Usage: \n \
		tp0 -h \n \
		tp0 -V \n \
		tp0 [options] \n \
		Options: \n \
		-V, --version Print version and quit.\n \
		-h, --help Print this information.\n \
		-i, --input Location of the input file.\n \
		-o, --output Location of the output file.\n \
		-a, --action Program action: encode (default) or decode.\n \
		Examples:\n \
		tp0 -a encode -i ~/input -o ~/output\n \
		tp0 -a decode \n");
}

void show_version(){
	printf("Organizacion de Computadoras - TP0 \n \
			Encoder/Decoder Base64 - v2.0 \n \
			Group Members:\n \
			Gonzalez Perez, Ailen Padron: 97043\n \
			Mariotti, Maria Eugenia Padron: 96260\n \
			Raña, Cristian Ezequiel Padron: 95457\n");
}

int leer_entrada(int input_fd, char* buffer, int bytes) {
	while (read(input_fd, buffer, 1) && --bytes) 
		buffer++;
	return bytes;
}

void b64_decode(char *in, char *out){
		char encoded[4] ={0};
		int i;
		int l;
		
		for (i = 0;i<4;++i){
			for (l = 0; l<64;++l){
					if (in[i] == letters_table[l]){
						encoded[i] = l;
						break;
					}
			}
		}
		
		out[0] = (encoded[0] << 2) + ((encoded[1] & 0x30) >> 4);
		out[1] = ((encoded[1] & 0xf) << 4) + ((encoded[2] & 0x3c) >> 2);
		out[2] = ((encoded[2] & 0x3) << 6) + encoded[3];
}

void b64_encode(char *in, char *out){
	char decoded[4] = {0};
	
	decoded[0] = in[0] >> 2;
	decoded[1] = ((in[0] % 4) << 4) | (in[1] >> 4);
	decoded[2] = ((in[1] % 16) << 2) | (in[2] >> 6);
	decoded[3] = in[2] % 64;

	out[0] = letters_table[(int) decoded[0]];
	out[1] = letters_table[(int) decoded[1]];
	out[2] = letters_table[(int) decoded[2]];
	out[3] = letters_table[(int) decoded[3]];
}

int main(int argc, char* argv[]){
	
	bool help, version, output, input, decode; //posibles opciones
	help = version = output = input = decode = false; //arrancan todas en falso por default
	char *input_file, *output_file;
	input_file = output_file = NULL; //para tener los nombres de archivo. If 0 -> standar
	int fd_input, fd_output;
	fd_input = 0; //file descriptor de stdin es 0.
	fd_output = 1; // file descriptor de stdout es 1.

	for (int i=0; i<argc; i++){
		char *arg = argv[i];
		if (strcmp(arg,"-v")==0 || strcmp(arg,"--version")==0){
				version = true;
		}
		if (strcmp(arg,"-h")==0 || strcmp(arg,"--help")==0){
				help = true;
		}
		if (strcmp(arg,"-o")==0 || strcmp(arg,"--output")==0){
				output_file = argv[++i];
				if (strcmp(output_file,"-")!=0)
					output = true; // - es std. No debo hacer el open
		}
		if (strcmp(arg,"-i")==0 || strcmp(arg,"--input")==0){
				input_file = argv[++i];
				if (strcmp(input_file,"-")!=0)
					input = true; // - es std. No debo hacer el open
		}
		if (strcmp(arg,"-a")==0 || strcmp(arg,"--action")==0){
				if (strcmp(argv[++i],"decode")==0)
					decode = true;
		}
	}

	int exit = 0;
	
	if (help) {
		show_help();
		return exit; //show and quit
	}
	
	if (version) {
		show_version();
		return exit; //show and quit
	}
	
	if (input){ //si hay input, lo abro
		fd_input = open(input_file, O_RDONLY); //obtengo el fd	de input (sólo lectura)	
	}
	if (output){
		fd_output = open(output_file, O_RDWR|O_CREAT, S_IRWXU); //obtengo el fd	(podría leer y escribir)
	}											//chequear si tercer campo hace falta
	
	if (fd_output == -1 || fd_input == -1){ //Error de archivos de apertura
		printf("Falla en apertura de archivos. Intente nuevamente. \n");
		exit = -1;
		return exit;
	}
		
	//HASTA ACÁ, PREPROCESAMIENTO.
	//PASO A LO PROPIO DE BASE 64
	
	int bytesPendientes = 0;
	int bytesALeerMax = 3;
	int bytesAEscribirMax = 3;
	if (decode)
		bytesALeerMax= 4; //El decode hace de 4 bytes, 3 nuevos
	else
		bytesAEscribirMax = 4; //Si es encode (not decode), hace 4 bytes de 3 originales
	bool fin = false;
	
	while (!bytesPendientes && !fin){ //
	
			char entrada[4] = {0};
			char salida[4] = {0};
			
			//Leo los bytes
			bytesPendientes = leer_entrada(fd_input,entrada,bytesALeerMax); //Me guardo en entrada, los 4 bytes
			if (bytesPendientes == bytesALeerMax){ //Se terminó el archivo (no disminuyó con el --)
				bytesPendientes=0;
				break; //Salgo
			}	
			
			int padding = 0;
			for (int j = 0; j <bytesALeerMax; j++){
				if (entrada[j] == '=')
					padding++;
			}
			int aEscribir = bytesAEscribirMax - bytesPendientes - decode * padding; //if decode = false -> no suma el padding
			
			if (entrada[3] == (char)'=')
				fin = true;
			
			if (decode)
				b64_decode(entrada,salida); //Llamada a decode
			else
				b64_encode(entrada,salida); //Llamada a encode
				
			//Ahora, escribo lo que me dejó en salida
			
			write(fd_output,salida,aEscribir);
	}
	
	char symbol = '=';
	while(bytesPendientes--)
		write(fd_output,&symbol,1); //Relleno con = lo que falte
	
	close(fd_input);
	close(fd_output);
	return exit;
}
