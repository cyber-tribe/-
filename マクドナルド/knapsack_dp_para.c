#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* Thread間の信号 */
#define RED 1
#define GREEN 0

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
static int** sign;  /* 次のスレッドに処理が終了したことを伝える */
static int n;  /* 品数 */
static int capacity;   /* 支払い可能な金額 */
static int threads;   /* スレッド数 */

/* 関数の宣言 */
void initProducts();  /* productsを初期化する */
void allocateDP(void);  /* dpにメモリを割り当てる */
void deallocateDP(void);  /* dpのメモリを解放する */
void initDP(void);  /* dpを初期化する */
void allocateSign(void);  /* signにメモリを割り当てる */
void deallocateSign(void);  /* signのメモリを解放する */
void initSign(void);  /* signを初期化する */
void showSolution();  /* 最適解の品物リストを表示する */
void showDP(void);  /* dpの中身を表示する */
int allClear(int i, int threadNum);  /* threadが動作可能かを調べる */
void *threadFunc(void* arg);  /* Thread内で動かす関数(knapsack) */

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
    scanf("%d",&threads);
    initProducts();
    allocateDP();
    initDP();
    allocateSign();
    initSign();
    pthread_t* threadList = (pthread_t*)malloc(threads*sizeof(pthread_t));

    /* 計算部 */
    t1 = gettimeofday_sec();
    for(int i=0;i<threads;i++){
        pthread_create(&threadList[i], NULL, threadFunc, (void *)i);
    }
    pthread_join(threadList[threads-1], NULL);
    t2 = gettimeofday_sec();

    /* 出力部 */
    printf("time:%.8f\n", t2 - t1);
    while((opt = getopt(argc, argv, "abc :")) != -1){
        /* -a: パラメータを表示, -b: 最適解の品物リストを表示, -c: dpの中身を表示 */
        switch (opt) {
            case 'a':
                printf("n=%d, capacity=%d, threads=%d\n",n,capacity,threads);
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

    /* メモリの解放 */
    deallocateDP();
    deallocateSign();

    return 0;
}

/* 関数の定義 */
void initProducts(){
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
void allocateSign(void){
    sign = (int**)malloc((n+1)*sizeof(int*));
    sign[0] = (int*)malloc((threads)*(n+1)*sizeof(int));
    for(int i=0;i<n+1;i++){sign[i] = sign[0]+i*(threads);}
}
void deallocateSign(void){
    for(int i=0;i<n+1;i++){
        sign[i] = NULL;
        free(sign[i]);
    }
    sign = NULL;
    free(sign);
}
void initSign(void){
    for(int i=0;i<n+1;i++){
        for(int j=0;j<threads;j++){
            if(i==0){sign[i][j] = GREEN;}
            else{sign[i][j] = RED;}
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

int allClear(int i, int threadNum){
    int sum=0;
    for(int j=0;j<threadNum;j++){
        sum += sign[i][j];
    }
    return sum;
}

void *threadFunc(void* arg){
    int threadNum = (int)arg;
    int startIndex = (capacity/threads)*threadNum;
    int endIndex = startIndex+(capacity/threads)-1;
    if(threadNum == threads-1){endIndex=capacity;}
    for(int i=0;i<=n;i++){
        if(threadNum==0){}
        else{
            while(allClear(i, threadNum) != GREEN){
                /* 赤信号の間は待機する */
                usleep(1);}
        }
        for(int j=startIndex;j<=endIndex;j++){
            if(i==0 || j==0){ dp[i][j].num = 0;}
            else if(j >= products[i-1].price){
                int a = dp[i-1][j-products[i-1].price].num + products[i-1].kcal;
                int b = dp[i-1][j].num;
                if(a > b){
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
        sign[i][threadNum] = GREEN;
    }
    return NULL;
}
