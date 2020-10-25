PAV - P2: detección de actividad vocal (VAD)
============================================

>En esta práctica se intentará llevar a cabo un reconocedor de voz, diferenciando dentro de un fichero audio .wav las tramas que corresponden a señal de voz y las que corresponden a silencio o ruido. Se continuará con algunos de los algoritmos desarrollados en la práctica 1, como la tasa de cruce por ceros o la potencia media que se utilizaran para poder diferenciar entre las tramas de silencio y voz, también será interesante poder ver estos conceptos de teoría gráficamente, para así poder reforzar nuestros conocimientos sobre la interpretación de ficheros de audios. 


Ejercicios
----------

### Etiquetado manual de los segmentos de voz y silencio

- Etiquete manualmente los segmentos de voz y silencio del fichero grabado al efecto. Inserte, a 
  continuación, una captura de `wavesurfer` en la que se vea con claridad la señal temporal, el contorno de
  potencia y la tasa de cruces por cero, junto con el etiquetado manual de los segmentos.

>Empezando por la gráfica de arriba, el orden es el siguiente: ZCR, POT, AMP y señal .wav. Hemos usado el algoritmo de las práctica 1 para obtener las gráficas. 


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/1.jpeg)

- A la vista de la gráfica, indique qué valores considera adecuados para las magnitudes siguientes:

* Incremento del nivel potencia en dB, respecto al nivel correspondiente al silencio inicial, para estar seguros de que un segmento de señal se corresponde con voz.
	  
>Como se puede ver, el valor de este primer pico es de -36.56 y se trata de ruido, que se debería considerar como Silencio, teniendo en cuenta esto y que el resto de picos más altos todos son de voz, cualquier valor del umbral por encima de este serviria para asegurar que un segmento es de Voz y no de Silencio.


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/11.jpeg)

* Duración mínima razonable de los segmentos de voz y silencio.
	
>Como podemos ver en la imagen, la length mínima aproximada que podemos obtener de Silencio es de 0.091 segundos
>Para determinar la mínima length aproximada de un segmento Voz seguimos el mismo procedimiento y obtenemos 0.552 segundos.
>Podemos ver como esta gran diferencia entre tamaño de tramas será muy importante para poder mejorar el algoritmo.


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/12.jpeg)

* ¿Es capaz de sacar alguna conclusión a partir de la evolución de la tasa de cruces por cero?

>A parte de poder apreciar como en los silencios la tasa se eleva mucho, podemos ver cómo en algunos fonemas concretos se llega a unos valores pico muy elevados. Estos fonemas son mayoritariamente las fricativas |s| y |f| esto puede ser de gran ayuda ya que de esta manera podemos garantizar que si alguna trama tiene un valor de zrc muy elevado, por mucho que no supere el umbral de potencia, ha de ser considerado como Voz ya que se tratará de una fricativa.


### Desarrollo del detector de actividad vocal

- Complete el código de los ficheros de la práctica para implementar un detector de actividad vocal tan
  exacto como sea posible. Tome como objetivo la maximización de la puntuación-F `TOTAL`.

>El código está en el github.

- Inserte una gráfica en la que se vea con claridad la señal temporal, el etiquetado manual y la detección
  automática conseguida para el fichero grabado al efecto. 

![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/2.png)



- Explique, si existen. las discrepancias entre el etiquetado manual y la detección automática.

>Como podemos observar en la siguiente captura, a veces hay una breve diferencia temporal entre la manual (la transcription inferior) y la automática (la superior) y a veces no existe una diferencia notable. No pensamos que dicha diferencia influya demasiado en el correcto funcionamiento de nuestro programa.


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/3.png)


>Lo que si que repercute es que, algunas veces, sin practicamente haber fluctuaciones en el audio, se produce el siguiente fenómeno


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/4.png)


>Como vemos, en el trascription automático se puede apreciar unos cambios de estado sin sentido. 


- Evalúe los resultados sobre la base de datos `db.v4` con el script `vad_evaluation.pl` e inserte a 
  continuación las tasas de sensibilidad (*recall*) y precisión para el conjunto de la base de datos (sólo
  el resumen).
  
![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/5.jpeg)


>Como se puede ver, en nuestro audio tenemos una tasa de acierto bastante elevada **(94.139%)** esto es debido a que grabamos un audio sin apenas ruido de fondo y sin demasiadas _“complicaciones”_ para el programa. Como _"complicaciones"_ se entiende palabras acabadas en oclusivas, silencios muy cortos o tramas de voz  cortas, tal y como explicaremos más adelante, cualquiera de las características anteriores puede llegar a repercutir mucho a la hora de analizar una señal.
Por otra parte, analizando la sensibilidad y la precisión, podemos apreciar como el porcentaje de la precisión es más elevado que el de la sensibilidad, pero en ambos casos el porcentaje final es bastante elevado. Más adelante y teniendo en cuenta el resto de ficheros .wav analizaremos más detenidamente la sensibilidad y precisión de nuestro programa.


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/6.png)


>El resultado final de analizar todos los ficheros de la carpeta db.v4 es de **89.576%**, que pese a ser un valor bastante elevado, no es tan óptimo como desearíamos o como hemos obtenido en nuestro fichero .wav. Esto es debido a bastantes elementos.

>Empezaremos hablando de la **sensibilidad y precisión** del programa. 

>Tras analizar todos los ficheros, consideramos que donde más problemas aparecen es en la sensibilidad. Por un lado, entendemos que es así ya que uno de los mayores problemas que consideramos que tiene nuestro algoritmo es la no identificación de consonantes oclusivas... Cuando una palabra acaba con este tipo de fonemas, nuestro algoritmo no está totalmente implementado para identificarlo y los considera como Silence cuando en realidad es trata de Voice! ¿Por qué sucede esto? Bien, las consonantes oclusivas tienen la característica de que provocan una breve interrupción en la señal de voz, por lo tanto el programa lo suele interpretar como silencio, aunque forme parte de la voz. Por este motivo, la sensibilidad en algunos ficheros con bastantes palabras acabadas en estos fonemas pueden presentar una tasa de sensibilidad muy baja (más adelante se expone un ejemplo). Por otro lado, el ruido de fondo también repercute negativamente en los resultados, puesto que al no haber llegado a desarrollar de forma total los umbrales, si un pico de ruido los supera, podría llegar a ser considerado como Voz en vez de Silencio y esto también afecta a la hora de obtener la tasa de la sensibilidad. Como se pide en la Ampliación 1, una muy buena opción hubiese sido poner todas las tramas de Silencio a 0 para así conseguir una mayor claridad entre Silencio y Voz y que ningún pico de ruido se acercara a los umbrales, pero eso lo explicaremos más detalladamente en la Ampliación 1. 

>Por lo que a la precisión respecta, las dificultades también van mucho en la misma línea que con la sensibilidad, pero como acertamos muchas más tramas de las que erramos, la precisión es más fácil de conseguir un valor elevado (siempre y cuando el fichero sea un poco largo).

>Otro aspecto que nos gustaría tener en cuenta es que sin llegar a estar implementado totalmente, deseábamos que nuestro algoritmo fuese capaz de detectar destellos de ruido y no considerarlos como Voz. El razonamiento que hemos intentado llevar a cabo, es que si una trama de Voz de duración muy reducida se encontraba entre tramas de Silencio, claramente se debería tratar de una falsa alarma (destello de ruido) y pese a que se pudiera llegar a interpretar como Voz, ya que superaría el umbral, teniendo en cuenta que se encuentra entre tramas de Silencio y que su duración es muy reducida, se debería considerar como una trama de Silencio y no como Voz.

>Hemos analizado un audio del cual tenemos una tasa de acierto muy baja (50.873%). 


![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/7.jpeg)

>El audio es la siguiente frase: **Hola sóc l'Albert**

>Uno de los motivos de tener una baja tasa de aciertos sería que una de las solo 3 palabras que forman el audio, acaba en oclusiva \[t] (concretamente sorda). Que una de las únicas 3 palabras que se dicen en el audio, sea oclusiva y sorda, baja mucho la tasa de acierto.

>Una forma de solucionar esto es utilizando la tasa de cruces por cero. Como se puede apreciar en la primera gráfica adjunta, los fonemas que con la medida de la potencia media podrían dar problemas (los cuales su valor de potencia está por debajo del umbral) con la medida de su tasa de cruces por cero queda muy claro que se tratan de tramas de Voz y no de Silencio cómo podría parecer. De esta manera, también es muy importante tener en cuenta que si una trama que se está considerando como posible silencio tiene su tasa de cruces por cero  mayor que un cierto umbral (definido por nosotros mismos), esa trama será Voz y no influirá si su valor de potencia está por encima del umbral (k0) o no.				


Trabajos de ampliación
----------------------

#### Cancelación del ruido en los segmentos de silencio

- Si ha desarrollado el algoritmo para la cancelación de los segmentos de silencio, inserte una gráfica en
  la que se vea con claridad la señal antes y después de la cancelación (puede que `wavesurfer` no sea la
  mejor opción para esto, ya que no es capaz de visualizar varias señales al mismo tiempo).
  
> Hemos desarrollado la siguiente solución para esta ampliación:

> Al principio del código, rellenamos el fichero de salida con los datos. 

```c
if (sndfile_out != 0) {
      sf_write_float(sndfile_out, buffer, frame_size);
      /* TODO: copy all the samples into sndfile_out */
    }
```
>Hacia el final, en el caso de que el estado sea silencio, escribimos ceros. 

```c
if (sndfile_out != 0) {
      /* TODO: go back and write zeros in silence segments */
        if(state == ST_SILENCE ){
        sf_write_float(sndfile_out, buffer_zeros, frame_size);
      }
    }
```
>Al poner el siguiente comando en la terminal:

				bin/vad -i pav_4181.wav -o output.vad -w output.wav 
				
>generamos el audio de salida y no entendemos por qué motivo es exactamente igual al de entrada. Es decir, parece que no rellena con ceros los silencios. 

>Consideramos que muy probablemente el motivo por el que no rellena con ceros los silencios es que no se puede reescribir en un fichero ya anteriormente se ha rellenado tan fácilmente con solo poniendo: 

				sf_write_float(sndfile_out, buffer_zeros, frame_size);. 

>Habíamos pensado en utilizar un fichero auxiliar y rellenar la partes de Voz con buffer y las partes de Silencio con buffer_zeros, pero tras varios intentos y diferentes formas de llevarlo a cabo, al no lograrlo, decidimos centrarnos más en la segunda ampliación. Aunque sí que nos gustaría remarcar que sabemos que lograr un fichero final con los Silencios puestos a cero lograria una mucho mayor nitidez de la señal de audio y una anulación prácticamente total del ruido de fondo.


#### Gestión de las opciones del programa usando `docopt_c`

- Si ha usado `docopt_c` para realizar la gestión de las opciones y argumentos del programa `vad`, inserte
  una captura de pantalla en la que se vea el mensaje de ayuda del programa.


### Contribuciones adicionales y/o comentarios acerca de la práctica

- Indique a continuación si ha realizado algún tipo de aportación suplementaria (algoritmos de detección o 
  parámetros alternativos, etc.).

- Si lo desea, puede realizar también algún comentario acerca de la realización de la práctica que
  considere de interés de cara a su evaluación.
  

Conclusiones
------------
>En esta segunda práctica, hemos aprendido a trabajar con herramientas como el GitHub o el docopt y hemos extendido nuestros conocimientos en otras que habíamos aprendido en la práctica 1 como el Wavesurfer. También ha servido para repasar muchos conceptos de teoría y ver gráficamente la importancia de la tasa de cruces o la potencia media.

>Nos gustaría comentar que, tal y como hemos ido mencionando durante la memoria, algunas de las partes de la práctica no hemos podido llegar a implementar los algoritmos hasta el nivel deseado, pero que ha sido más por un tema de dificultades con el lenguaje C ya que muchos de los apartados que no hemos conseguido implementar, los hemos desarrollado muy extensamente en esta memoria explicando cuales serian las pautas a seguir para poderlos llevar a cabo y cuales serian los conceptos a tener en cuenta en cada punto.

>En relación al contenido de la práctica, nos ha resultado sorprendente la facilidad con la que se puede conseguir un reconocedor de voz bastante preciso pero de la misma manera, una vez se consigue un nivel alto de reconocimiento de voz, es igualmente sorprendente lo difícil que es mejorar el algoritmo. Hay muchos fonemas muy difíciles de ser identificados correctamente, como por ejemplo los oclusivos, y esto se podría enlazar con lo interesante que sería ver cómo respondería este mismo algoritmo a otro idioma... ya que otra cosa que nos hemos dado cuenta es que el reconocimiento de voz debe variar mucho entre diferentes idiomas, sobre todo si proceden de diferentes lenguas madre. De esta manera, una futura vía de investigación sería la nombrada anteriormente, utilizar nuestro reconocedor de voz con ficheros de otro idioma y comparar el resultado con un algoritmo fabricado para ese idioma en concreto, seguramente las diferencias serían muy notables.

>Finalmente, querríamos añadir una posible mejora que nos hubiese gustado implementar, la cual encontramos útil. La idea sería agrupar frames del mismo tipo consecutivos, para así reducir la tasa de error. Como se ve en la siguiente imagen, el resultado sería así (lo hemos modificado a mano).

![imagen ](https://github.com/manelcardenas/PAV-P2/blob/Rodriguez-Cardenas/otra.png)





