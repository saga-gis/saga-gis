
typedef struct simple_PIXEL_list
{
   unsigned short row;
   unsigned short col;
   double greyvalue;
   
   struct simple_PIXEL_list *next;
}
simple_PIXEL_list;


typedef struct double_PIXEL_list
{
   unsigned short row;
   unsigned short col;
   double greyvalue;
   
   struct double_PIXEL_list *prev;
   struct double_PIXEL_list *next;
}
double_PIXEL_list;




typedef struct simple_REGION_list
{
   struct simple_REGION_list *next;

   struct simple_PIXEL_list *first;
   struct simple_PIXEL_list *last;
}
simple_REGION_list;


typedef struct double_REGION_list
{
   struct double_REGION_list *prev;
   struct double_REGION_list *next;

   struct double_PIXEL_list *first;
   struct double_PIXEL_list *last;
}
double_REGION_list;



int geodesic_morphological_reconstruction (
                unsigned short numrows,
                unsigned short numcols,
                double **mask,
                double **marker);

