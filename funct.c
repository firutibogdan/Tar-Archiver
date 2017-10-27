#include <stdio.h>
#include <string.h>

int str2int(char *a) {
	int x = 0, n, i;
	n = strlen(a);
	for(i = 0; i < n; i++) {
		x *= 10;
		x += (a[i] - '0');
	}
	return x;
}

int oct2int(char *a) {
        int x = 0, i;
        int b = 1;
        i = strlen(a);
        i--;
        for(i; i >= 0; i--) {
                x = x + b * (a[i] - '0');;
                b = b * 8;
        }
        return x;
}

void int2oct(int x, char *a) {
	char aux[12];
	int i = 0, j;
	while(x > 0) {
		aux[i] = x % 8 + '0';
		x = x / 8;
		i++;
	}
	i--;
	j = 0;
	for(i ; i >= 0; i--) {
		a[j] = aux[i];
		j++;
	}
	a[j] = '\0';
	
}

void permissions(char *a, char *b) {
	
	int p1 = 0, p2 = 0, p3 = 0;
	if(a[1] == 'r') p1 += 4;
	if(a[2] == 'w') p1 += 2;
	if(a[3] == 'x') p1 += 1;
	if(a[4] == 'r') p2 += 4;
        if(a[5] == 'w') p2 += 2;
        if(a[6] == 'x') p2 += 1;
	if(a[7] == 'r') p3 += 4;
        if(a[8] == 'w') p3 += 2;
        if(a[9] == 'x') p3 += 1;
	b[0] = p1 + '0';
	b[1] = p2 + '0';
	b[2] = p3 + '0';
	b[3] = '\0';
}
