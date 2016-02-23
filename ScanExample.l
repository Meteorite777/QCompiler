%{
	int myPow(int base, int exponent){
		int answer = 1;
		while(exponent--){
			answer = answer * base;
		}
		return answer;
	}

	int octalToDecimal(int octal){
		int decimal = 0;
		int i = 0;
		int rem;
		while(octal != 0){
			rem = octal%10;
			octal /= 10;
			decimal += rem * myPow(8, i);
			++i;
	}
	return decimal;
}
%}

Decimal 	[1-9][0-9]*
Octal 		0[0-7]+

%%

{Octal}		{ printf("%d\n", octalToDecimal(atoi(yytext))); }
{Decimal}	{ printf("%s\n", yytext); }
.		{ }

%%



