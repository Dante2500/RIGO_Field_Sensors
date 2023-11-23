#include "encrypt_lib.h"

String codificacion(float data[], int length) {
  //String pre = "AT+SEND=2:";
  //String post = "\r\n";
  String payload = "";

  for (int i = 0; i < length; i++) {
    // Aplica la conversión a cada elemento en la lista
    int intValue = (int)(data[i] * 10) + 656;
    String hexStr = String(intValue, HEX);
    String nox = hexStr.substring(0);

    if (nox.length() % 2 != 0) {
      // Añade un 0 a la izquierda si la longitud no es par
      nox = "0" + nox;
    }

    payload += nox;
  }

  //String code = pre + payload + post;
  return payload;
}


void decodificacion(String hexData, float data[], int length) {
  // Asegúrate de que la longitud de la cadena hexData sea suficiente
  /**/
  int posInicial = hexData.indexOf(':');
  int posFinal = hexData.indexOf(':', posInicial + 1);
  posFinal = hexData.indexOf(':', posFinal + 1);
  posFinal = hexData.indexOf(':', posFinal + 1);

  // Verificar si se encontraron tres ':' en la cadena
  if (posFinal != -1) {
    // Obtener la subcadena a partir del tercer ':'
    String resultado = hexData.substring(posFinal + 1);
    
    // Imprimir el resultado
    Serial.println("Resultado: " + resultado);
    if (resultado.length() < length * 4) {
      Serial.println("La longitud de la cadena hexData es insuficiente.");
    }

  // Recorre la cadena hexData y convierte los datos hexadecimales a decimales
    for (int i = 0; i < length; i++) {
      String hexValue = resultado.substring(i * 4, (i + 1) * 4);
      long intValue = strtol(hexValue.c_str(), NULL, 16);
      data[i] = (float)(intValue - 656) / 10.0;
    }


  } else {
    Serial.println("No se encontraron cuatro ':' en la cadena");
    return;
  }

  
}