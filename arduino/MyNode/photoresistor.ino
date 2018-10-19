void loopPS() {
  photoresistor = analogRead(A0);
  Serial.print("PS:");
  Serial.println(photoresistor);
}
