// DO NOT INCLUDE ANY OTHER LIBRARIES/FILES
#include "pointer.h"

// In this assignment, you can assume that function parameters are valid and the memory is managed by the caller

// Compares the price of obj1 with obj2
// Returns a negative number if the price of obj1 is less than the price of obj2
// Returns a positive number if the price of obj1 is greater than the price of obj2
// Returns 0 if the price of obj1 is equal to the price of obj2
int compare_by_price(Object* obj1, Object* obj2)
{
    double p1 = object_price(obj1);
    double p2 = object_price(obj2);

    if(p1 == p2){
        return 0;
    }else if(p1 < p2){
        return -1;
    }else{
        return 1;
    }
    return 0;
}

// Compares the quantity of obj1 with obj2
// Returns a negative number if the quantity of obj1 is less than the quantity of obj2
// Returns a positive number if the quantity of obj1 is greater than the quantity of obj2
// Returns 0 if the quantity of obj1 is equal to the quantity of obj2
int compare_by_quantity(Object* obj1, Object* obj2)
{
    unsigned int q1 = object_quantity(obj1);
    unsigned int q2 = object_quantity(obj2);

    if(q1 == q2){
        return 0;
    }else if(q1 < q2){
        return -1;
    }else{
        return 1;
    }
    return 0;
}

// Initializes a StaticPriceObject with the given quantity, name, and price
// Memory for the name string is managed by the caller and assumed to be valid for the duration of the object's lifespan
void static_price_object_construct(StaticPriceObject* obj, unsigned int quantity, const char* name, double price)
{
    obj->obj.virtual_func_table.price = (price_fn)static_price;
    obj->obj.virtual_func_table.bulk_price = (bulk_price_fn)static_bulk_price;
    obj->obj.quantity = quantity;
    obj->obj.name = name;
    obj->price = price;
}

// Initializes a DynamicPriceObject with the given quantity, name, base price, and price scaling factor
// Memory for the name string is managed by the caller and assumed to be valid for the duration of the object's lifespan
void dynamic_price_object_construct(DynamicPriceObject* obj, unsigned int quantity, const char* name, double base, double factor)
{
    obj->obj.virtual_func_table.price = (price_fn)dynamic_price;
    obj->obj.virtual_func_table.bulk_price = (bulk_price_fn)dynamic_bulk_price;
    obj->obj.quantity = quantity;
    obj->obj.name = name;
    obj->base = base;
    obj->factor = factor;
}

// Returns the price of a StaticPriceObject or ERR_OUT_OF_STOCK if it is out of stock
double static_price(StaticPriceObject* obj)
{
    if(obj->obj.quantity == 0){
        return ERR_OUT_OF_STOCK;
    }else{
        return obj->price;
    }
    return 0;
}

// Returns the price of a DynamicPriceObject or ERR_OUT_OF_STOCK if it is out of stock
// The dynamic price is calculated as the base price multiplied by (the quantity raised to the power of the scaling factor)
double dynamic_price(DynamicPriceObject* obj)
{
    if(obj->obj.quantity == 0){
        return ERR_OUT_OF_STOCK;
    }else{
        double multiplier = pow(obj->obj.quantity, obj->factor);
        return obj->base * multiplier;
    }
    return 0;
}

// Returns the bulk price of purchasing multiple (indicated by quantity parameter) StaticPriceObject at a discount where the first item is regular price and the additional items are scaled by the BULK_DISCOUNT factor
// Return ERR_OUT_OF_STOCK of there is insufficient quantity available
double static_bulk_price(StaticPriceObject* obj, unsigned int quantity)
{
    if(obj->obj.quantity < quantity){
        return ERR_OUT_OF_STOCK;
    }else if(quantity == 1){
        return static_price(obj);
    }else if(quantity == 0){
        return 0;
    }else{
        int i;
        double total = 0;

        total += static_price(obj);
        for(i = 1; i < quantity; i++){
            total += static_price(obj) * BULK_DISCOUNT;
        }
        return total;
    }
    return 0;
}

// Returns the bulk price of purchasing multiple (indicated by quantity parameter) DynamicPriceObject at a discount where the first item is regular price and the additional items are scaled by the BULK_DISCOUNT factor
// This uses the same dynamic price equation from the dynamic_price function, and note that the price changes for each item that is bought
// For example, if 3 items are requested, each of them will have a different price, and this function calculates the total price of all 3 items
// Return ERR_OUT_OF_STOCK of there is insufficient quantity available
double dynamic_bulk_price(DynamicPriceObject* obj, unsigned int quantity)
{
    if(obj->obj.quantity < quantity){
        return ERR_OUT_OF_STOCK;
    }else if(quantity == 1){
        return dynamic_price(obj);
    }else if(quantity == 0){
        return 0;
    }else{
        int i;
        double total = 0;

        total += dynamic_price(obj);
        unsigned int tempqnt = obj->obj.quantity;
        obj->obj.quantity -= 1;

        for(i = 0; (quantity - 1) > 0; i ++){
            total += dynamic_price(obj) * BULK_DISCOUNT;
            obj->obj.quantity -= 1;
            quantity -= 1;
        }
        obj->obj.quantity = tempqnt;
        return total;
    }
    return 0;
}

//
// Iterator functions
//

// Initializes an iterator to the beginning of a list
void iterator_begin(LinkedListIterator* iter, LinkedListNode** head)
{
    iter->curr = *head;
    iter->prev_next = head;
}

// Updates an iterator to move to the next element in the list if possible
void iterator_next(LinkedListIterator* iter)
{
    if(iter->curr != NULL){
        iter->prev_next = &iter->curr->next;
        iter->curr = iter->curr->next;
    }
}

// Returns true if iterator is at the end of the list or false otherwise
// The end of the list is the position after the last node in the list
bool iterator_at_end(LinkedListIterator* iter)
{
    if(iter->curr == NULL){
        return true;
    }
    return false;
}

// Returns the current object that the iterator references or NULL if the iterator is at the end of the list
Object* iterator_get_object(LinkedListIterator* iter)
{
    if(iter->curr != NULL){
        return iter->curr->obj;
    }
    return NULL;
}

// Removes the current node referenced by the iterator
// The iterator is valid after call and references the next object
// Returns removed node
LinkedListNode* iterator_remove(LinkedListIterator* iter)
{
    struct LinkedListNode_s* deleted = iter->curr;
    *iter->prev_next = iter->curr->next;
    iter->curr = iter->curr->next;
    return deleted;
}

// Inserts node after the current node referenced by the iterator
// The iterator is valid after call and references the same object as before
// Returns ERR_INSERT_AFTER_END error if iterator at the end of the list or 0 otherwise
int iterator_insert_after(LinkedListIterator* iter, LinkedListNode* node)
{
    if(iterator_at_end(iter)){
        return ERR_INSERT_AFTER_END;
    }else{
        node->next = iter->curr->next;
        iter->curr->next = node;
    }
    return 0;
}

// Inserts node before the current node referenced by the iterator
// The iterator is valid after call and references the same object as before
void iterator_insert_before(LinkedListIterator* iter, LinkedListNode* node)
{
    *iter->prev_next = node;
    node->next = iter->curr;
    iter->prev_next = &(node->next);
}

//
// List functions
//

// Returns the maximum, minimum, and average price of the linked list
void max_min_avg_price(LinkedListNode** head, double* max, double* min, double* avg)
{
    LinkedListNode* iter = *head;
    int i;
    *avg = 0;
    *max = object_price(iter->obj);
    *min = object_price(iter->obj);
    for(i = 0; iter != NULL; i++){
        if(object_price(iter->obj) > *max){
            *max = object_price(iter->obj);
        }
        if(object_price(iter->obj) < *min){
            *min = object_price(iter->obj);
        }
        *avg += object_price(iter->obj);
        iter = iter->next;
    }
    *avg = *avg / i;
}

// Executes the func function for each node in the list
// The function takes in an input data and returns an output data, which is used as input to the next call to the function
// The initial input data is provided as a parameter to foreach, and foreach returns the final output data
// For example, if there are three nodes, foreach should behave like: return func(node3, func(node2, func(node1, data)))
Data foreach(LinkedListNode** head, foreach_fn func, Data data)
{
    LinkedListNode* iter = *head;
    int i;

    for(i = 0; iter != NULL; i++){
        data = func(iter->obj, data);
        iter = iter->next;
    }
    return data;
}

// Returns the length of the list
int length(LinkedListNode** head)
{
    LinkedListNode* iter = *head;
    if(iter == NULL){
        return 0;
    }

    int count = 0;
    while(iter != NULL){
        count += 1;
        iter = iter->next;
    }
    return count;
}

//
// Mergesort
//

// Assuming list1 and list2 are sorted lists, merge list2 into list1 while keeping it sorted
// That is, when the function returns, list1 will have all the nodes in sorted order and list2 will be empty
// The sort order is determined by the compare function
// Default convention for compare functions on objects A and B:
//   Negative return values indicate A should be earlier than B in the list
//   Positive return values indicate A should be later than B in the list
//   Zero return values indicate A and B are equal
// A stable sort is not required for this implementation, so equal objects can be in either order
void merge(LinkedListNode** list1_head, LinkedListNode** list2_head, compare_fn compare)
{
    LinkedListIterator iterA;
    iterator_begin(&iterA, list1_head);
    LinkedListIterator iterB;
    iterator_begin(&iterB, list2_head);

    while(!iterator_at_end(&iterB)){
        if(compare(iterator_get_object(&iterB), iterator_get_object(&iterA)) < 0){
            iterator_insert_before(&iterA, iterator_remove(&iterB));
        }else if(compare(iterator_get_object(&iterB), iterator_get_object(&iterA)) == 0){
            iterator_insert_before(&iterA, iterator_remove(&iterB));
        }else if(iterator_at_end(&iterA)){
            iterator_insert_before(&iterA, iterator_remove(&iterB));
        }else{
            iterator_next(&iterA);
        }
    }
    return;
}

// Split the list head in half and place half in the split list
// For example, if head has 8 nodes, then split will move 4 of them to split_head
void split(LinkedListNode** head, LinkedListNode** split_head)
{
    LinkedListIterator iterA;
    iterator_begin(&iterA, head);
    LinkedListIterator iterS;
    iterator_begin(&iterS, split_head);

    int l = length(head) / 2;
    int i;
    for(i = 0; i < l; i++){
        iterator_insert_before(&iterS, iterator_remove(&iterA));
        iterator_next(&iterS);
    }
}

// Implement the mergesort algorithm to sort the list
// The sort order is determined by the compare function
// Default convention for compare functions on objects A and B:
//   Negative return values indicate A should be earlier than B in the list
//   Positive return values indicate A should be later than B in the list
//   Zero return values indicate A and B are equal
// A stable sort is not required for this implementation, so equal objects can be in either order
void mergesort(LinkedListNode** head, compare_fn compare)
{
    LinkedListNode* headsort = NULL;
    if(length(head) > 1){
        split(head, &headsort);
        mergesort(head, compare);
        mergesort(&headsort, compare);
        merge(head, &headsort, compare);
    }
}
