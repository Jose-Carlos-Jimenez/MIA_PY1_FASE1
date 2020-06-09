echo "Compilando BISON..."
bison -o parser.cpp --defines=parser.h sintactico.y
echo "Compilando FLEX..."
flex -o scanner.cpp --header-file=scanner.h lexico.l
echo "Generación del compilador terminada..."
