/*----------------------------------------
  Pantalla de escucha de puerto Serial
  ----------------------------------------*/
void loopSerial() {
  // Variable donde se almacena lo que llega por el serial
  unsigned int a;

  // bloque que detecta si se alcanzo el final de carrera
  if (digitalRead(endstopPin) == endstopPRESSED) {
    // trabaja con un flag para no repetir
    if (flagEndstopSerial == false) {
      noTone(pulsePin);
      digitalWrite(enablePin, currentDISABLE);
      lcd.setCursor(0, 1);
      lcd.print("- EndstopReached       ");
      Serial.println("Aborted Endstop Reached");
      flagEndstopSerial = true;
    }
  }
  else
    flagEndstopSerial = false;

  // escucha el puerto serial
  if (Serial.available() > 0) {
    a = Serial.parseInt();
    // si llego algo distinto a 0
    // y el mensaje anterior fue "6" (step)
    if (a > 0 && flagNumberSteps) {      
      flagNumberSteps = false;
      // escucha el numero de pasos a ejecutar
      lcd.setCursor(8, 1);
      lcd.print(a);
      lcd.print("         ");
      // ejecuta los pasos
      for (int i = 0; i < a; i++) {
        digitalWrite(pulsePin, LOW);
        delayMicroseconds(delaySTEPSSERIAL);
        digitalWrite(pulsePin, HIGH);
        delayMicroseconds(delaySTEPSSERIAL);
        // comprueba que no se alcanze el final de carrera
        if (digitalRead(endstopPin) == endstopPRESSED)
          break;
      }
      // da a aviso del final de la ejecucion
      Serial.print(a);
      Serial.println(" steps executed");
    }
    // si llego algo distinto a 0
    // y el mensaje anterior fue "10" (New Calibration)
    else if (a > 0 && flagNewCalibration){
      flagNewCalibration = false;
      calibration = a;
      EEPROM.put(calibrationMEMLOC, calibration);
      updateLimits();
      // da a aviso de la nueva calibracion
      Serial.println("New Calibration: ");
      Serial.print(a);
    }
    // si el flag no esta levantado y a>30, 
    // entonces a se interpreta como una nueva frecuencia
    else if (a > 30) {
      if (a > MAX_FREQ){
        lcd.setCursor(0, 1);
        lcd.print("Invalid frequency: ");
        Serial.print("Invalid frequency: ");
        Serial.print(a);
      }
      else {
        tone(pulsePin, a);
        lcd.setCursor(0, 1);
        lcd.print(a);
        lcd.print(" Hz          ");
        Serial.print("New frequency established: ");
        Serial.print(a);
        Serial.println(" Hz");
      }
    }
    // los valores menores a 30 tienen significado especial
    else {
      switch (a) {
        case 1:
          lcd.setCursor(0, 1);
          lcd.print("1 Disable        ");
          digitalWrite(enablePin, currentDISABLE);
          Serial.println("Current Disabled");
          break;
        case 2:
          lcd.setCursor(0, 1);
          lcd.print("2 Enable        ");
          digitalWrite(enablePin, currentENABLE);
          Serial.println("Current Enabled");
          break;
        case 3:
          lcd.setCursor(0, 1);
          lcd.print("3 Forward        ");
          digitalWrite(directionPin, directionFORWARD);
          Serial.println("Direction: Forward");
          break;
        case 4:
          lcd.setCursor(0, 1);
          lcd.print("4 Backward        ");
          digitalWrite(directionPin, directionBACKWARD);
          Serial.println("Direction: Backward");
          break;
        case 5:
          lcd.setCursor(0, 1);
          lcd.print("5 No Tone        ");
          noTone(pulsePin);
          Serial.println("Generator Off");
          break;
        case 6:
          lcd.setCursor(0, 1);
          lcd.print("6 Steps: listen");
          Serial.println("Listening number of steps...");
          noTone(pulsePin);
          flagNumberSteps = true;
          break;
        case 7:
          lcd.setCursor(0, 1);
          lcd.print("7 Return Cal");
          Serial.println("Calibration (periods/mm): ");
          Serial.println(calibration);
          Serial.println("Syringe Length (micrometer): ");
          Serial.println(syringeLength);
          Serial.println("Syringe Volume (microliter): ");
          Serial.println(syringeVolume);
          Serial.println("Flowrate (microliter/hour): ");
          Serial.println(flowrate);
          Serial.println("Total Volume (microliter): ");
          Serial.println(totalVolume);
          Serial.println("Total Time (seconds): ");
          Serial.println(totalTime);
          Serial.println("Minimum Flowrate (microliter/hour): ");
          Serial.println(minFlowRate); 
          Serial.println("Maximum Flowrate (microliter/hour): ");
          Serial.println(maxFlowRate);
          Serial.println("Minimum Total Time (seconds): ");
          Serial.println(minTotalTime);
          Serial.println("Maximum Total Time (seconds): ");
          Serial.println(maxTotalTime);
          break;
        case 8:
          lcd.setCursor(0, 1);
            lcd.print("8 Version");
          Serial.println("Firmware:");
          Serial.println(firm);
          break;
        case 9:
          Serial.println("pong");
          lcd.setCursor(0, 1);
          lcd.print("9 Ping");
          break;
        case 10:
          lcd.setCursor(0, 1);
          lcd.print("10 New Cal");
          Serial.println("Listening Calibration number...");
          noTone(pulsePin);
          flagNewCalibration = true;
          break;
      }
    }
  }
  switch (lcd_key) {
    case btnSELECT:
      digitalWrite(directionPin, directionFORWARD);
      digitalWrite(enablePin, currentDISABLE);
      screen = scrMAIN;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Flow Stopped");
      delay(msgDELAY);
      break;
  }
}
