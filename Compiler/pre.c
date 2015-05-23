
# 1 ../stdio.h 1



int printf(const char*,...);
int scanf(const char*,...);
# 1 ../factorial.c 2


long factorialRecursion(int n){
return n>1?n*factorialRecursion(n-1):1;
}

long factorialFor(int n){
long r=1;
for(;n;--n){
r*=n;
}
return r;
}

long factorialWhile(int n){
long r=1;
while(n){
r*=n;
--n;
}
return r;
}

long factorialDoWhile(int n){
long r=1;
int i=1;
do{
r*=i;
++i;
}while(i<=n);
return r;
}

long factorialWhileBreak(int n){
long r=1;
while(1){
r*=n;
--n;
if(!n)
break;
}
return r;
}

long factorialForBreak(int n){
long r=1;
for(;;){
r*=n;
--n;
if(!n)
break;
}
return r;
}


int main(){
int a;
printf("Zadejte faktorial: ");
scanf("%d",&a);
if(a>10){
printf("%d > 10: nelze vypocitat\n",a);
return 1;
}

printf("cislo %d je %s\n",a,a%2?"liche":"sude");

printf("%d! = %d (rekurze)\n",a,factorialRecursion(a));
printf("%d! = %d (cyklus for)\n",a,factorialFor(a));
printf("%d! = %d (cyklus for+break)\n",a,factorialForBreak(a));
printf("%d! = %d (cyklus while)\n",a,factorialWhile(a));
printf("%d! = %d (cyklus while+break)\n",a,factorialWhileBreak(a));
printf("%d! = %d (cyklus do-while)\n",a,factorialDoWhile(a));
return 0;
}