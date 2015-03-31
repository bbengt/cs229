#ifdef __cplusplus
extern "C" {
#endif

#ifndef UTILS_H
# define UTILS_H

# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

int makedirectory(char *dir);
int roll_dice(char *dice);

#endif

#ifdef __cplusplus
}
#endif