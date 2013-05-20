PureData = {
  /**
   * Configures connection with audio hardware with given sample rate, 
   * number of channels and enables or disables mixing (wether sound 
   * stays on on when app ins minimized).
   * Parameters can be changed so success callback should check for new values.
   * 
   * Example:
   *     
   *      PureData.configurePlayback(44100, 2, false, false, 
   *         function(params){
   *           // Success, do something with params.sampleRate, 
   *           // params.numChannels, params.inputEnabled, params.mixingEnabled
   *         },
   *         function(error){
   *           console.log(error);
   *           navigator.notification.alert('Sorry, I can\'t start audio playback :(', null, null, null);
   *         }
   *       );
   *  
   */
  configurePlayback: function(sampleRate, numberChannels, inputEnabled, mixingEnabled, success, error){
    cordova.exec(success, error, "PureData", "configurePlayback", [sampleRate, numberChannels, inputEnabled, mixingEnabled]);
  },
  

  /**
   * Opens a PD patch. 
   */
  openFile: function(dir, file, success, error){
    cordova.exec(success, error, "PureData", "openFile", [dir, file]);
  },

  /**
   * Start/Stop sound I/O.
   */
  setActive: function(active, success, error){
    cordova.exec(success, error, "PureData", "setActive", [active]);
  },

  /**
   * Send a message to the PD patch
   */
  sendFloat: function(num, receiver){
    cordova.exec(null, null, "PureData", "sendFloat", [num, receiver]);
  },

  /**
   * Send a MIDI note on message to the PD patch
   */
  sendNoteOn: function(note, velocity){
    cordova.exec(null, null, "PureData", "sendNoteOn", [note, velocity]);
  }
}
