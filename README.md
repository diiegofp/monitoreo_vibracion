# Monitor de vibración para equipo rotativo 

Este proyecto es un sistema de monitoreo de vibración en tiempo real diseñado para el mantenimiento predictivo de maquinaria. Utiliza un microcontrolador **STM32F302R8** (ARM Cortex-M4) para procesar datos de aceleración y detectar posibles fallas mecánicas. Ademas, fue mi primer gran acercamiento a desarrollar una aplicacion embebida manejando de manera precisa los recursos del micro.

## Hardware utilizado
* **Microcontrolador:** STM32F302R8.
* **Sensor:** Acelerómetro triaxial LIS3DH (vía I2C).
* **Pantalla:** OLED SH1106 de 1.3" (vía I2C).
* **Procesamiento:** Uso de la FPU para cálculos de magnitud vectorial.

## Características Principales
* **Cálculo en Tiempo Real:** Gracias a la FPU habilitada, el sistema calcula la magnitud vectorial $\sqrt{x^2 + y^2 + z^2}$ sin retrasar el flujo del programa.
* **Arquitectura obusta:** Implementación mediante una **Máquina de Estados Finitos (FSM)** para separar las tareas de lectura, procesamiento y visualización.
* **Bus I2C compartido:** Tanto el sensor como la pantalla conviven en el mismo bus (PB8-SCL y PB9-SDA).

## 📂 Estructura del Código
* `main.c`: logica con maquina de estados
* `fsm.c/h`: flujo para el control de la fsm
* `acelerometro.c/h`: configuracion y lectura del acelerometro.
* `sh1106.c/h`: gestion de la pantalla

## Resultados
El sistema clasifica la vibración en tres estados según la magnitud detectada:
1. **OK:** Vibración normal o reposo.
2. **MEDIO:** Alerta de revisión necesaria.
3. **ALTO:** Peligro de falla inminente.

---

## DDEMO
Puedes ver el sistema funcionando en vivo, desde la inicialización hasta las pruebas de vibración crítica en el siguiente enlace:

[![Ver Video en YouTube](https://img.shields.io/badge/YouTube-Ver%20Video-red?style=for-the-badge&logo=youtube)](https://youtu.be/Ijlfkwhf0U4)

---

## Contacto


* **Nombre:** [Diego Iboy]
* **Correo:** [diego.iboy@galileo.edu]
