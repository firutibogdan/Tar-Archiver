#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "funct.h"

union record {
	char charptr[512];
	struct header {
		char name[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12]; 
		char chksum[7];
		char typeflag;
		char linkname[101];
		char magic[8];
		char uname[32];
		char gname[32];
		char devmajor[8];
		char devminor[8];
	} header;
};

union record r, res;

int main() {
	
	FILE *fin, *fin2, *out;
	char cmd[512], *archivename, *filename, *txt, line[512], line2[512],
	     	*p = NULL, user[40], group[40], date[11], time[30], *q, *aux;
	char *delim=" \n";
	int year, month, day, hour, min, second, uidd, gidd, i, sz, 
		count = 0, s, offset, length;

	/*folosesc time.h pentru a calcula diferenta dintre data si ora fisierului
		si 1 ian 1970 00:00, transform in octal numarul de secunde si il
		adaug in vectorul r.header.mtime */	
	struct tm data, ref;
	int sec;
	ref.tm_hour = 0;
        ref.tm_min = 0;
        ref.tm_sec = 0;
        ref.tm_year = 70;
        ref.tm_mon = 0;
        ref.tm_mday = 1;
	
	//citesc fiecare comanda
	fgets(cmd, 511, stdin);
	while(strncmp(cmd, "quit", 4) != 0) {
		if(strncmp(cmd, "load", 4) == 0) {
			
			//iau numele arhivei
			archivename = strtok(cmd, delim);
			archivename = strtok(NULL, delim);
			
			//parcurg file_ls (daca pot sa deschid)
			fin = fopen("file_ls","rt");
			if(fin != NULL) {
	
				/*citesc fiecare linie din file_ls pentru a afla
					metadatele fisierului ce trebuie adaugat */				
				fgets(line, 511, fin);
				while(! feof(fin)) {
					
					//s o sa fie suma octetilor din header
					//suma ce trebuie adaugata in r.header.chksum
					s = 0;
					r.header.typeflag = ' ';
					s += r.header.typeflag; 
					strcpy(r.header.magic ,"GNUtar ");
					s = s + 'G' + 'N' + 'U'  + 't' + 'a' + 'r' + ' ';
					
					//folosesc strtok pentru parsarea textului	
					p = strtok(line, delim);
	
					aux = (char *) calloc(3, sizeof(char));
					//trec permisiunile in mod octal
					permissions(p, aux);
					strcpy(r.header.mode, "0000");
					strcat(r.header.mode, aux);
					for(i = 0; i < 7; i++) {
						s += r.header.mode[i]; 
					}					
					free(aux);
	
					p = strtok(NULL, delim);	
					p = strtok(NULL, delim);
				
					//pastrez userul in user
					strcpy(user, p);
					strcpy(r.header.uname, p);
					length = strlen(r.header.uname);
					for(i = 0; i < length; i++) {
						s += r.header.uname[i];
					}
	
					p = strtok(NULL, delim);
					
					//pastrez grupul in group
					strcpy(group, p);
					strcpy(r.header.gname, p);
					length = strlen(r.header.gname);
	                                for(i = 0; i < length; i++) {
						s += r.header.gname[i];
					}

				
					p = strtok(NULL, delim);
				
					//sz este maimea fisierului
					sz = str2int(p);
					aux = (char *) calloc(12, sizeof(char));
					int2oct(sz, aux);
					for(i = 0; i < 11 - strlen(aux); i++) strcat(r.header.size, "0");
					strcat(r.header.size, aux);
					length = strlen(r.header.size);
	                                for(i = 0; i < length; i++) {
						s += r.header.size[i];
					}

					free(aux);
				
					p = strtok(NULL, delim);
				
					strcpy(date, p);
				
					p = strtok(NULL, delim);
				
					strcpy(time, p);
				
					//pastrez data si ora
					p = strtok(NULL, delim);
					p = strtok(NULL, delim); 
				
					q = strtok(date, "-");
		                        year = str2int(q);
					q = strtok(NULL, "-");
		                        month = str2int(q);
		                        q = strtok(NULL, "-");
		                        day = str2int(q);
		                        q = strtok(time, ":");
		                        hour = str2int(q);
		                        q = strtok(NULL, ":");
					min = str2int(q);
					q = strtok(NULL, ".");
					second = str2int(q);
					data.tm_hour = hour - 1;
					data.tm_min = min;
					data.tm_sec = second;
					data.tm_year = year - 1900;
					data.tm_mon = month - 1;
	       		 		data.tm_mday = day;
					/*folosesc time.h pentru a calcula numarul
					  de secunde trecute din 1970, numar pe care
					  il trec in octal si il retin in r.header.mtime */
					sec = difftime(mktime(&data), mktime(&ref));
					aux = (char *) calloc(12, sizeof(char));
					int2oct(sec, aux);
					length = strlen(aux);
					for(i = 0; i < 11 - length; i++) {
						strcat(r.header.mtime, "0");
					}
					strcat(r.header.mtime, aux);
					length = strlen(r.header.mtime);
		                        for(i = 0; i < length; i++) {
						s += r.header.mtime[i]; 
					}

					free(aux);
		
					//pun numele fisierului in r.header.name
					strcpy(r.header.name, p);
					
					length = strlen(r.header.name);
		                        for(i = 0; i < length; i++) {
						s += r.header.name[i]; 
					}
					strcpy(r.header.linkname, "0"); 
					s += '0';
					//parcurg usermap.txt pentru a gasi uid si gid
					fin2 = fopen("usermap.txt", "rt");
					if(fin2 != NULL) {
						fgets(line2, 511, fin2);
						while(! feof(fin2)) {
							p = strtok(line2, ":");
							if(strcmp(p, user) == 0) {
					                        p = strtok(NULL, ":");
								p = strtok(NULL, ":");
								//al doilea camp este uid
								uidd = str2int(p);

								p = strtok(NULL, ":");
								//al treilea camp este gid
								gidd = str2int(p);
								
								//copiez uid si il trec in octal
								aux = (char *) calloc(8, sizeof(char));
								int2oct(gidd, aux);
								length = strlen(aux);
								for(i = 0; i < 7 - length; i++) {
									strcat(r.header.gid, "0");
								}
								strcat(r.header.gid, aux);
								length = strlen(r.header.gid);
						                for(i = 0; i < length; i++) {
									s += r.header.gid[i];
								}

								free(aux);
								aux = (char *) calloc(8, sizeof(char));
								int2oct(uidd, aux);
								length = strlen(aux);
								for(i = 0; i < 7 - length; i++) {
									strcat(r.header.uid, "0");
								}
								strcat(r.header.uid, aux);
								length = strlen(r.header.uid);
					                	for(i = 0; i < length; i++) {
									s += r.header.uid[i];
								}

								free(aux);
						
								break;
							}
							fgets(line2, 511, fin2);
						}
						fclose(fin2);

				                for(i = 0; i < 7; i++) {
							s += ' '; 
						}
						aux = (char *) malloc(7 * sizeof(char));
						int2oct(s, aux);
						length = strlen(aux);
						for(i = length; i < 6 ; i++) {
							strcat(r.header.chksum, "0");
						}
						strncat(r.header.chksum, aux, length);
						free(aux);

	
						//deschid arhiva facuta de mine
						if(count == 0) {
							/*daca e prima oara vreau suprascriu
							  datele care exista */ 
							out = fopen(archivename, "wb");
							count++;
						} else {
							//daca nu, adaug la ea cu append
							out = fopen(archivename, "ab");
						}
		
						//voi scrie un articol de tip record (metadatele)
						fwrite(&r, sizeof(union record), 1, out);
						FILE *f;
						f = fopen(r.header.name, "rb");
						//deschid fiserul si copiez continutul
						txt = (char *) calloc (513, sizeof(char));
						fread(txt, sizeof(char), 512, f);
						while(! feof(f)) {
							fwrite(txt, sizeof(char), 512, out);
							free(txt);
							txt = (char *) calloc (513, sizeof(char));
							fread(txt, sizeof(char), 512, f);
						}
						fwrite(txt, sizeof(char), 512, out);
						free(txt);
						/*resetez record-ul r la starea initiala
						unde erau doar 0-uri */
						r = res; 
						fgets(line, 511, fin);
						if(feof(fin)) {
				                        txt = (char *) calloc (513, sizeof(char));
				                        fwrite(txt, sizeof(char), 512, out);
				                        fwrite(txt, sizeof(char), 512, out);

							free(txt);
				                }
						fclose(out);

					} else {
						printf("Eroare! Nu exista usermap.txt");
					}
				}
				fclose(fin);
			} else {
				printf("Eroare! Nu exista file_ls");
			}
		} else {
			if(strncmp(cmd, "list", 4) == 0) {
				archivename = strtok(cmd, delim);
                        	archivename = strtok(NULL, delim);
				fin = fopen(archivename, "rb");
				
				if(fin != NULL) {
					fread(&r, sizeof(union record), 1, fin);
					while(! feof(fin) && r.header.name[0] != 0) {
						fputs(r.header.name, stdout);
						printf("\n");
						offset = oct2int(r.header.size);
						if(offset % 512 != 0) {
							offset = (offset / 512 + 1) * 512;
						}
						fseek(fin, offset, SEEK_CUR);
						fread(&r, sizeof(union record), 1, fin);
					}
					fclose(fin);
				
				} else {
					printf("Eroare! Nu exista arhiva");
				}
	
			} else {
				if(strncmp(cmd, "get", 3) == 0) {
					archivename = strtok(cmd, delim);
	                                archivename = strtok(NULL, delim);
					filename = strtok(NULL, delim);
					fin = fopen(archivename, "rb");
					
					if(fin != NULL) {
						/*parcurg fiecare record in cautarea 
						  lui filename */
						fread(&r, sizeof(union record), 1, fin);
						while(! feof(fin) && r.header.name[0] != 0) {
							
							/*offset imi spune peste cati	
							  octeti sa sar (dimensiunea fiecarui
							  fiser gasit */
                	                        	offset = oct2int(r.header.size);
							//offset e multiplu de 512
                        	                	if(offset % 512 != 0) {
                                                		offset = (offset / 512 + 1) * 512;
                                	        	}
							if(strcmp(r.header.name, filename) == 0) {

								/*cand am gasit fiserul
								  iau continutul sau din arhiva
								  si il afisez la stdout */
								txt = (char *) calloc(513, sizeof(char));
								fread(txt, sizeof(char), 512, fin);
								/*count va fi numarul de blocuri
								  de lungime 512 ce cu date din
								  fisier */
								count = 1;
								while(count <= offset / 512) {
									if(count == offset / 512) {
										/*daca este ultimul bloc
										  este posbil sa nu fie 512
										  caractere, asa ca voi lua 
										  512 - diferenta dintre 
										  offset si dimensiunea reala
										  a fisierului */
										fwrite(txt, sizeof(char), 512 - offset + oct2int(r.header.size), stdout);
									} else {
										fwrite(txt, sizeof(char), 512, stdout);
									}
									free(txt);
									txt = (char *) calloc(513, sizeof(char));
									fread(txt, sizeof(char), 512, fin);
									count++;
								}
								free(txt);
                                	                        break;
                                        	        }
	
							//sar peste offset octeti
        	                                	fseek(fin, offset, SEEK_CUR);
                	                        	fread(&r, sizeof(union record), 1, fin);
                        	        	}
						fclose(fin);
					} else {
						printf("Eroare! Nu exista arhiva");
					}				
				}
			}
		}
		fgets(cmd, 511, stdin);
	}
	return 0;
}
