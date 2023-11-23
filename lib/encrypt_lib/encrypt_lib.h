#ifndef ENCRYPT_LIB_H
#define ENCRYPT_LIB_H

#include <Arduino.h>

String codificacion(float data[], int length);
void decodificacion(String hexData, float data[], int length);
#endif