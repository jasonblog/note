/*test special global char*  */
char *string_nested_if_for = "\nPASS NESTED FOR with IF statement!\n\n";
char *string_fact = "\nPASS Factorial test!\n\n";
char *string_fib = "\nPASS Fibonacci test!\n\n";
char *string_gcd = "\nPASS GCD test!\n\n";

void nested_if_for_test()
{
    int nest_num = 3;
    int nest_index;
    int inner_nest_num = 2;
    int inner_nest_index;
    for(nest_index = 0;nest_index < nest_num;)
    {
        if( 100 < 5)
        {
            printf("JCC failed 1!\n");
        }
        else
        {
            printf("pass first \"if statement\" in %d \"for statement\"\n",nest_index);
            if(-20 < 50)
            {
                printf("pass second \"if statement\" in %d \"for statement\"\n",nest_index);
                for(inner_nest_index = 0;inner_nest_index < inner_nest_num;)
                {
                    printf("pass %d \"inner for statement\" in %d \"for statement\"\n",inner_nest_index,nest_index);
                    inner_nest_index = inner_nest_index + 1;
                }
            }else
            {
                printf("JCC failed 2!\n");
            }
        }
        nest_index = nest_index +1;
    }
}


void factorial_test()
{
    int n, i;
    int factorial = 1;

    printf("Enter an integer for factorial test: \n");
    scanf("%d",&n);

    /*test nested for statement inside if statement*/
    if (n < 0)
        printf("Error! Factorial of a negative number doesn't exist.\n");
    else
    {
        for(i=1; i<=n;)
        {
            factorial = factorial * i;
            i = i + 1;
        }
        printf("Factorial of %d is: %d\n", n, factorial);
    }
}

void iterative_fibobacci()
{
    int term;
	int first = 0, second = 1, next, i;
 
    printf("Please input the term number for Iterative Fibonacci Sequence:\n");
    scanf("%d",&term);
   	printf("The first %d terms of Fibonacci Sequences are :\n",term);
 
    /*test nested if statement inside for statement*/
   	for ( i = 0 ; i < term ; )
   	{
    	if ( i <= 1 )
		{
        	next = i;
		}
      	else
      	{
            next = first + second;
            first = second;
            second = next;
      	}
      	printf("%d term -> %d\n",i,next);
        i =i+1;
    }
}

/*test function call with return value*/
int gcd(int a, int b)
{
    int c;
    for(;a != 0;)
    {
        c = a;
        a = b%a;
        b = c;
    }
    return b;
}

void gcd_test()
{
    int i,a,b;
    int result;
    printf("GCD TEST will print the gcd from your input:\n");
    printf("Please input two integer:\n");
    scanf("%d %d",&a,&b);
    result = gcd(a,b);
    printf("The result of GCD(%d,%d) is %d\n",a,b,result);
}


void main()
{
    /*test special local cahr* */
    char *local_string = "JCC pass all the tests!\n\n";

    /*test function call without return value*/
    nested_if_for_test();
    printf("%s",string_nested_if_for);
    factorial_test();
    printf("%s",string_fact);
    iterative_fibobacci();
    printf("%s",string_fib);
    gcd_test();
    printf("%s",string_gcd);

    printf("%s",local_string);
}

