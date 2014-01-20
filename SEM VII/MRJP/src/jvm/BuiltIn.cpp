#include "BuiltIn.h"

namespace JVM {

std::ostream& writeBuiltIns(std::ostream& stream, std::string name) {
  stream << ".class public " << name << "\n\
.super java/lang/Object\n\
\n\
.field private static _scan Ljava/util/Scanner;\n\
\n\
\n\
.method public <clinit>()V\n\
.limit stack 3\n\
  new java/util/Scanner\n\
  dup\n\
  getstatic java/lang/System/in Ljava/io/InputStream;\n\
  invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n\
  putstatic " << name << "/_scan Ljava/util/Scanner;\n\
  return\n\
.end method\n\
\n\
\n\
; standard initializer\n\
.method public <init>()V\n\
.limit stack 1\n\
  aload_0\n\
  invokespecial java/lang/Object/<init>()V\n\
  return\n\
.end method\n\
\n\
\n\
; printInt\n\
.method public static printInt(I)V\n\
.limit stack 2\n\
  getstatic java/lang/System/out Ljava/io/PrintStream;\n\
  iload_0\n\
  invokestatic java/lang/Integer/toString(I)Ljava/lang/String;\n\
  invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n\
  return\n\
.end method\n\
\n\
\n\
; printString\n\
.method public static printString(Ljava/lang/String;)V\n\
.limit stack 2\n\
  getstatic java/lang/System/out Ljava/io/PrintStream;\n\
  aload_0\n\
  invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n\
  return\n\
.end method\n\
\n\
\n\
; error\n\
.method public static error()V\n\
.limit stack 1\n\
  ldc \"error\"\n\
  invokestatic " << name << "/printString(Ljava/lang/String;)V\n\
  iconst_1\n\
  invokestatic java/lang/System/exit(I)V\n\
  return\n\
.end method\n\
\n\
\n\
; readInt\n\
.method public static readInt()I\n\
.limit stack 5\n\
  new java/util/Scanner\n\
  dup\n\
  getstatic " << name << "/_scan Ljava/util/Scanner;\n\
  invokevirtual java/util/Scanner/nextLine()Ljava/lang/String;\n\
  invokespecial java/util/Scanner/<init>(Ljava/lang/String;)V\n\
  invokevirtual java/util/Scanner/nextInt()I\n\
  ireturn\n\
.end method\n\
\n\
\n\
; readString\n\
.method public static readString()Ljava/lang/String;\n\
.limit stack 3\n\
.limit locals 1\n\
  getstatic " << name << "/_scan Ljava/util/Scanner;\n\
  invokevirtual java/util/Scanner/nextLine()Ljava/lang/String;\n\
  areturn\n\
.end method\n\
\n\
\n\
; main method\n\
.method public static main([Ljava/lang/String;)V\n\
.limit stack 1\n\
.limit locals 1\n\
  invokestatic " << name << "/main()I\n\
  invokestatic java/lang/System/exit(I)V\n\
  return\n\
.end method" << std::endl << std::endl << std::endl;

  return stream;
};

}
