#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* 構造体の定義 */
typedef struct{
    char name[64];
    int kcal;
    int price;
} Product;
typedef struct DP{
    int num;
    int i;
    int j;
    struct DP* _parent;  /* 移動元 */
} DP;

/* グローバル変数の宣言 */
static Product* products;  /* 品物一覧 */
static DP** dp;
static int n;  /* 品数 */
static int capacity;  /* 支払い可能な金額 */

/* 関数の宣言 */
void initProducts(void);  /* productsを初期化する */
void allocateDP(void);  /* dpにメモリを割り当てる */
void deallocateDP();  /* dpのメモリを解放する */
void initDP(void);  /* dpを初期化する */
void knapsack(void);  /* knapsack法 */
void showSolution(void);  /* 最適解の品物リストを表示する */
void showDP(void);  /* dpの中身を表示する */


double gettimeofday_sec()  /* 時間計測用 */
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char *argv[]){
    double t1, t2;  /* 時間計測用 */
    int opt;  /* option管理用 */
    opterr = 0;  /* getopt()のエラーメッセージを無効にする */

    /* 初期化 */
    scanf("%d",&n);
    scanf("%d",&capacity);
    initProducts();
    allocateDP();
    initDP();

    /* 計算部 */
    t1 = gettimeofday_sec();
    knapsack();
    t2 = gettimeofday_sec();

    /* 出力部 */
    printf("time:%.8f\n", t2 - t1);
    while((opt = getopt(argc, argv, "abc :")) != -1){
        /* -a: パラメータを表示, -b: 最適解の品物リストを表示, -c: dpの中身を表示 */
        switch (opt) {
            case 'a':
                printf("n=%d, capacity=%d\n",n,capacity);
                break;
            case 'b':
                showSolution();
                break;
            case 'c':
                showDP();
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] [-b] [-c]\n", argv[0]);
                break;
        }
    }

    /* メモリの開放 */
    deallocateDP(dp,n);

    return 0;
}
void initProducts(void){
    products = (Product*)malloc(n*sizeof(Product));
    for(int i=0;i<n-1;i++){
        scanf("%s",products[i].name);
        scanf("%d",&products[i].price);
        scanf("%d",&products[i].kcal);
    }
}
void allocateDP(void){
    dp = (DP**)malloc((n+1)*sizeof(DP*));
    dp[0] = (DP*)malloc((capacity+1)*(n+1)*sizeof(DP));
    for(int i=0;i<n+1;i++){dp[i] = dp[0]+i*(capacity+1);}
}
void deallocateDP(void){
    for(int i=0;i<n+1;i++){
        dp[i] = NULL;
        free(dp[i]);
    }
    dp = NULL;
    free(dp);
}
void initDP(void){
    for(int i=0;i<n+1;i++){
        for(int j=0;j<capacity+1;j++){
            dp[i][j].i = i;
            dp[i][j].j = j;
            dp[i][j].num = 0;
            dp[i][j]._parent = NULL;
        }
    }
}
void knapsack(void){
    for(int i=0;i<=n;i++){
        for(int j=0;j<=capacity;j++){
            if (i==0 || j==0){ dp[i][j].num = 0;}
            else if(j>=products[i-1].price){
                int a = dp[i-1][j-products[i-1].price].num+products[i-1].kcal;
                int b = dp[i-1][j].num;
                if (a > b){
                    dp[i][j].num = a;
                    dp[i][j]._parent = &dp[i-1][j-products[i-1].price];
                }else{
                    dp[i][j].num = b;
                    dp[i][j]._parent = &dp[i-1][j];
                }
            }else{
                dp[i][j].num = dp[i-1][j].num;
                dp[i][j]._parent = &dp[i-1][j];
            }
        }
    }
}
void showSolution(void){
    int sum = 0;
    for(DP* head=&dp[n][capacity];head->_parent!=NULL;head=head->_parent){
        if(head->j != head->_parent->j){
            printf("%s\n",products[head->i-1].name);
            sum += products[head->i-1].price;
        }
    }
    printf("%d yen\n",sum);
    printf("%d Kcal\n",dp[n-1][capacity-1].num); 
}
void showDP(void){
    for(int i=0;i<n;i++){
        for(int j=0;j<capacity;j++){
            printf("%d ",dp[i][j].num);
        }
        printf("\n");
    }
}
