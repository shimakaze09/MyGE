java org.antlr.v4.Tool MyShader.g4
java org.antlr.v4.Tool -Werror -Dlanguage=Cpp -listener -visitor -package My::MyGE::details -o MyShader MyShader.g4javac *.java
java org.antlr.v4.Tool -Werror -Dlanguage=Cpp -listener -visitor -package My::MyGE::details -o MyShader MyShader.g4grun MyShader shader -gui < test.MyShader