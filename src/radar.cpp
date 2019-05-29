String s;

int start_millis = -1;
int end_millis = -1;
float top_speed = -1;


int radarCommand( String command ) {
    Serial1.println( command );
    return 1;
}

int reportParameter( String command ) {
    if( command == "wifi" )
        Particle.publish("report:wifi", WiFi.SSID() );
    if( command == "speed" )
        Particle.publish("speed", String(13.7) );
}

void setup() {
    // USB serial
    Serial.begin( 19200 );

    // radar serial
    Serial1.begin( 19200 );

    // expose functions to particle cloud
    Particle.function("radarCommand", radarCommand);
    Particle.function("reportParameter", reportParameter);
}

void loop() { 
  // the main loop mainly aggregates various reading into one, and sends this to the Particle cloud as an event
  if( Serial1.available() ) {
    char ch = Serial1.read();
    //Serial.write( ch );
    s += ch;
    if( ch == '\n' ) {
        
        float kph = -( s.toFloat() );
        if( kph != 0.0 ) {
            
            if( start_millis == -1 ) {
                start_millis = millis();
                end_millis = start_millis + 90000.0 / kph;
                top_speed = kph;
            } else {
                if( kph > top_speed ) {
                    end_millis = start_millis + 90000.0 / kph;
                    top_speed = kph;
                }
            }
            
            Serial.print( millis() );
            Serial.print( "\t" );
            Serial.print( kph );
            Serial.print( "\t" );
            Serial.println( millis() + 90000.0 / kph );
        } else {
            Serial.print( s );
            Particle.publish("debug:radar", s );
        }
        s = "";
    }
  }
  
  
  // publish the speed when readings have gone beyond their timing window
  if( ( end_millis != -1 ) && ( end_millis < millis() ) ) {
      Serial.print( "Report: " );
      Serial.print( top_speed );
      Serial.println( "kph" );
      
      Particle.publish("speed", String(top_speed), PUBLIC);
      
      start_millis = -1;
      end_millis = -1;
      top_speed = -1;
  }
  
}