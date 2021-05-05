#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ARGC_ERROR_MSG "Usage: RailwayPlanner <InputFile>"
#define FILE_NOT_FOUND_ERR "File does not exist."
#define EMPTY_FILE "File is empty."
#define INVALID_INPUT "Invalid input in line:"
#define FILE_NAME "rwp_output.out"
#define BUFFER_SIZE 1024 // number of characters to read in each fgets call
#define DELIMITERS "," // these are the delimiters for strtok - the characters to split by (space and tilde)
#define FILE_OPEN_MODE "r"
#define FILE_WRITE_MODE "w"

/**
 *This struct is used as a rail part
 */
typedef struct Node
{
    struct Node *next;
    unsigned int price ;
    unsigned int len;
    char start;
    char end;
} Node;

/**
 *This struct used as a list of rail parts
 */
typedef struct RailsList
{
    Node *head;
    Node *tail;
    char *connection_types;
    unsigned int length;
    unsigned int railway_size;
}RailsList;

/**
 *This function used to write error messages to the file
 * @param msg
 */
void writeToFileMsg(char *msg, int line)
{
    FILE *fp;
    fp = fopen(FILE_NAME, FILE_WRITE_MODE);
    if(!fp)
    {
        return;
    }
    //If it's invalid input error, than we add the number of the line
    if(!strcmp(msg, INVALID_INPUT))
    {
        fprintf(fp, "Invalid input in line: %d\n", line);
        fclose(fp);
        return;
    }
    //Any other error
    fprintf(fp, msg);
    fclose(fp);
}

/**
 *This function creates the rail list
 * @return
 */
RailsList *createRailsList()
{
    RailsList *new_list = NULL;
    new_list = (RailsList*) calloc(1, sizeof(RailsList));
    new_list->length = 0;
    new_list->head = NULL;
    return new_list;
}

/**
 *This function frees the RailsList object
 * @param list
 */
void freeRailList(RailsList *list)
{
    if(!list)
    {
        return;
    }
    while(list->head != NULL)
    {
        Node *next_node = list->head->next;
        free(list->head);
        list->head = next_node;
    }
    list->head = NULL;
    free(list->connection_types);
    list->connection_types = NULL;
}

/**
 *This function adds new node of rail object to the rail list
 * @param list - The list where to add
 * @param node - The node that added
 */
void addNodeToRailList(RailsList *list, Node *node)
{
    if(!list || !node)
    {
        return;
    }
    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        list->length = 1;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
        list->length++;
    }
}

/**
 * This function checks if the number that we got is good
 * @param num
 * @return
 * If the number is good return 0, o.w 1.
 */
int checkNumber(char *num)
{
    int len = (int)strlen(num);
    for(int i = 0; i < len; ++i)
    {
        char cur_char = num[i];
        if(cur_char == '\r' || cur_char == '\n')
        {
            continue;
        }
        if(num[i] < 48 || num[i] > 57 )
        {
            return 1;
        }
    }
    return 0;
}

/**
 *This function parsing the file and returns parsed rails list.
 * @param filename
 * @return
 */
RailsList *parseRailWay(const char *const filename)
{
    //Checking that the filename pointer exist
    if(!filename)
    {
        writeToFileMsg(FILE_NOT_FOUND_ERR, -1);
        return NULL;
    }
    FILE *fp = fopen(filename, FILE_OPEN_MODE);
    //Checking that the file was opened properly
    if(fp == NULL)
    {
        writeToFileMsg(FILE_NOT_FOUND_ERR, -1);
        return NULL;
    }
    RailsList *my_rail_list = createRailsList();
    char buffer[BUFFER_SIZE] = {0}, *token = NULL;
    if(fgets(buffer, BUFFER_SIZE, fp))
    {
        token = strtok(buffer, DELIMITERS); // set the string to parse, read first token
        //If we have any problem with the number.
        if(checkNumber(token))
        {
            writeToFileMsg(INVALID_INPUT, 1);
            freeRailList(my_rail_list);
            return NULL;
        }
        else
        {
            int rail_size = (int)strtol(token, &token, 10);
            my_rail_list->railway_size = rail_size;
        }
    }
    //If we dont have nothing on the first line than we assume that the file empty.
    else
    {
        writeToFileMsg(EMPTY_FILE, -1);
        freeRailList(my_rail_list);
        return NULL;
    }
    if(fgets(buffer, BUFFER_SIZE, fp))
    {
        my_rail_list->connection_types = NULL;
        my_rail_list->connection_types = (char*) calloc(1, sizeof(char));
        //This idx counts the number of connection elements
        int idx = 1;
        token = strtok(buffer, DELIMITERS);
        my_rail_list->connection_types[0] = token[0];
        while ((token = strtok(NULL, DELIMITERS))) // continue reading the last string put into strtok
        {
            char connection = *token;
            char *tmp = (char*) realloc(my_rail_list->connection_types, idx + 2);
            my_rail_list->connection_types = tmp;
            my_rail_list->connection_types[idx] = connection;
            idx++;
        }
    }
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        Node *new_node = NULL;
        new_node = (Node*) calloc(1, sizeof(Node));
        new_node->next = NULL;
        //This idx indicates on which part of the line its is (s,e,length,price)
        int idx = 1;
        token = strtok(buffer, DELIMITERS);
        new_node->start = token[0]; // The s type connection
        while ((token = strtok(NULL, DELIMITERS))) // continue reading the last string put into strtok
        {
            if(idx == 1)
            {
                new_node->end = token[0]; //The e type connection
            }
            else if(idx == 2)
            {
                if(checkNumber(token))//Checking that the length is correct
                {
                    writeToFileMsg(INVALID_INPUT, (int)my_rail_list->length + 3);
                    freeRailList(my_rail_list);
                    free(my_rail_list);
                    my_rail_list = NULL;
                    free(new_node);
                    new_node = NULL;
                    return NULL;
                }
                int cur_len = (int)strtol(token, &token, 10); //The length
                new_node->len = cur_len;
            }
            else if(idx == 3)
            {
                 if(checkNumber(token)) //Checking that the price is correct
                 {
                    writeToFileMsg(INVALID_INPUT, (int)my_rail_list->length + 3);
                    freeRailList(my_rail_list);
                    free(my_rail_list);
                    my_rail_list = NULL;
                    free(new_node);
                    new_node = NULL;
                    return NULL;
                 }
                int cur_price = (int)strtol(token, &token, 10); //The price
                new_node->price = cur_price;
            }
            //O.W if we have more than 4 elements, it's a mistake
            else
            {
                writeToFileMsg(INVALID_INPUT, (int)my_rail_list->length + 3);
                freeRailList(my_rail_list);
                free(my_rail_list);
                my_rail_list = NULL;
                free(new_node);
                new_node = NULL;
                return NULL;
            }
            idx++;
        }
        addNodeToRailList(my_rail_list, new_node); //Adding the node to the rails list
    }
    fclose(fp);
    return my_rail_list;
}

/**
 *This recursion function that helps the getMinPrice function.
 * @param list - The rails list
 * @param len - The length of the rail
 * @param con_typ - The e connection
 * @return - returns the min price
 */
unsigned int helperMinPrice(RailsList *list, unsigned int len, const char *con_typ)
{
    if(len == 0 || !con_typ || !list)
    {
        return 0;
    }
    char cur_char = (char)con_typ[0];
    unsigned int min_price  = INT_MAX;
    Node *tmp_node = list->head;
    while (tmp_node != NULL)
    {
        if(tmp_node->len <= len && tmp_node->end == cur_char)
        {
            //every iteration assign new length and connection type
            unsigned int cur_price = tmp_node->price + helperMinPrice(list, len - tmp_node->len, &tmp_node->start);
            if(min_price > cur_price)
            {
                min_price = cur_price; //If the new price smaller, change
            }
        }
        tmp_node = tmp_node->next; // Try another rail type
    }
    return min_price;
}

/**
 * This function returns the min price for the railway
 * @param table - The pointer to the table
 * @param list - The rails list
 * @return - The min price
 */
unsigned int getMinPrice(RailsList *list)
{
    if(!list)
    {
        return EXIT_FAILURE;
    }
    unsigned int len = list->railway_size; //Length of the railway
    int con_num = (int)strlen(list->connection_types); // Number of connections
    char *con_node = list->connection_types;
    unsigned int min_price = INT_MAX;
    //Going iterate over all the connection types like tha table
    for(int j = 0; j < con_num; ++j)
    {
        unsigned int cur_price = helperMinPrice(list, len, &con_node[j]);
        if(min_price > cur_price)
        {
            min_price = cur_price;
        }
    }
    return min_price;
}

/**
 *This function writes to the file the minimal price.
 * @param min_price - The minimal price
 */
void writeToFileMinPrice(unsigned int min_price)
{
    FILE *fp;
    fp = fopen(FILE_NAME, FILE_WRITE_MODE);
    if(!fp)
    {
        return;
    }
    if(min_price == INT_MAX) // If nothing were found
    {
        fprintf(fp, "The minimal price is: -1.");
    }
    else
    {
        fprintf(fp, "The minimal price is: %u.", min_price);
    }
    fclose(fp);
}

/**
 *The main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        writeToFileMsg(ARGC_ERROR_MSG, -1);
        return EXIT_FAILURE;
    }
    RailsList *my_list = parseRailWay(argv[1]);
    if(!my_list)
    {
        return EXIT_FAILURE;
    }
    unsigned int min_price = getMinPrice(my_list);
    writeToFileMinPrice(min_price);
    freeRailList(my_list);
    free(my_list);
    my_list = NULL;
    return 0;
}