#ifndef MUTEX_DS_H
#define MUTEX_DS_H

typedef struct mutex_ds_s * mutex_ds_p;

mutex_ds_p create_mutex_ds();

int get(mutex_ds_p mdp);

int put (mutex_ds_p mdp, int new_data);
#endif // MUTEX_DS_H
 
