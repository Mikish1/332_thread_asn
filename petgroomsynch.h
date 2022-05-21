typedef enum {cat, dog, other} pet_t;

int petgroom_init(int numstations);
int newpet(pet_t pet);
int petdone(pet_t pet);
int petgroom_done();
