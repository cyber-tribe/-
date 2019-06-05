#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char name[150];
    int price;
    int kcal;
} Product;

typedef struct shoppingList{
    Product* _product;
    struct shoppingList* _next;
} shoppingList;

Product* products;

void setProduct(int n);
void printProduct(int n);
void printList(shoppingList* head);
void append(shoppingList* head, int id);
int sizeList(shoppingList* head);
int sumPrice(shoppingList* head);
int sumKcal(shoppingList* head);
shoppingList* knapsack(int n,int capacity);

int main(){
    int n, capacity;
    scanf("%d",&n);
    scanf("%d",&capacity);
    products = (Product*)malloc(n*sizeof(Product));
    setProduct(n);
    printProduct(n);

    shoppingList* head = knapsack(n, capacity);
    printList(head);
    
    return 0;
    
}

void setProduct(int n){
    for(int i=0;i<n;i++){
        scanf("%s",products[i].name);
        scanf("%d",&products[i].price);
        scanf("%d",&products[i].kcal);
    }
}
void printProduct(int n){
    printf("[ Products ]\n");
    for(int i=0;i<n;i++){
        printf("%s ",products[i].name);
        printf("%d ",products[i].price);
        printf("%d\n",products[i].kcal);
    }
}
void printList(shoppingList* head){
    /* shoppingListの中身を標準出力する */
    printf("[ shoppingList ]\n");
    for(shoppingList* current = head;
        current != NULL;
        current = current->_next)
    {
        printf("%d, ",current->_product->price);
        printf("%d, ",current->_product->kcal);
        printf("%s\n",current->_product->name);
    }
    printf("Sum of Price: %d\n",sumPrice(head));
    printf("Sum of Kcal: %d\n",sumKcal(head));
}
void append(shoppingList* head, int id){
    /* shoppingListに新しい要素を追加する */
    if(head->_product==NULL){
        head->_product = &products[id];
        return;}
    if(head->_next==NULL){
        shoppingList* new = (shoppingList*)malloc(sizeof(shoppingList));
        new->_product = &products[id];
        new->_next = NULL;
        head->_next = new;
        return;
    }else{
        append(head->_next,id);
        return;
    }
}

int sizeList(shoppingList* head){
    int size = 0;
    for(shoppingList* current = head;
        current != NULL;
        current = current->_next)
    {
        size++;
    }
    return size;
}
int sumPrice(shoppingList* head){
    /* shoppingListのpriceの合計値を求める */
    if(head->_product == NULL){return 0;}
    int sum = 0;
    for(shoppingList* current = head;
        current != NULL;
        current = current->_next)
    {
        sum += current->_product->price;
    }
    return sum;
} 
int sumKcal(shoppingList* head){
    /* shoppingListのkcalの合計値を求める */
    if(head->_product == NULL){return 0;}
    int sum = 0;
    for(shoppingList* current = head;
        current != NULL;
        current = current->_next)
    {
        sum += current->_product->kcal;
    }
    return sum;
} 
shoppingList* knapsack(int n,int capacity){
    if(capacity==0){
        shoppingList* head = (shoppingList*)malloc(sizeof(shoppingList));
        head->_next = NULL;
        head->_product = NULL;
        return head;
        }
    else{
        shoppingList* head = knapsack(n,capacity-1);
        int payable = capacity - sumPrice(head);
        int lastTotalCal = sumKcal(head);
        int currentTotalCal = lastTotalCal;
        int index = -1;
        for(int i=0;i<n;i++){
            if(products[i].price <= payable){
                if (products[i].kcal+lastTotalCal >= currentTotalCal)
                {
                    currentTotalCal = lastTotalCal+products[i].kcal;
                    index = i;
                }
            }
        }
        if(index!=-1){append(head, index);}
        return head;
    }
}
