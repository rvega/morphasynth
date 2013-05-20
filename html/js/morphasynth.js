/*
 * Global namespace for the app
 */
if(typeof Morphasynth==='undefined') Morphasynth = {};

/*
 * Manages the options
 */
Morphasynth.Options = function(){
  this.x = 'envelope-a';
  this.y = 'mfcc-1';

  this.init = function(){
    $('#select-x').val(0);
    $('#select-y').val(1);
    $('#select-x, #select-y').bind('change', $.proxy(this.changedOption, this));
  }

  this.changedOption = function(event){
    var select = $(event.target);

    if(select.attr('id')=='select-x'){
      this.x = select.val();
      //Space.actPoints();
    }
    else{
      this.y = select.val();
      //Space.actPoints();
    }
  }
}

/*
 * Mock class, simulates the C++ app that contains the webkit instance where this script runs
 * If ran within the C++ app, this object will be provided to us
 */ 
// function createMockContainerApp(){
//   Morphasynth.ContainerApp = function(){
//     // this.oscConnector = null;
// 
//     this.init = function(){
//       this.proxyServer = {host: 'localhost', port: 1488};
//       this.udpHosts = [{host: 'localhost', port: 9999}];
//       this.handler = new K2.OSCHandler(this.proxyServer, this.udpHosts);
//       this.oscConnector = this.handler.registerClient({
//         clientID: "client1",
//         oscCallback : function (message) {
//           console.log ("client1 received message: ", message);
//         }
//       });
// 
//       var msg = new K2.OSC.Message('/hi/there/', 2, 3.0);
//       this.oscConnector.sendOSC(msg);
//     }
// 
//     this.sendMessage = function(msg, a ,b){
//       var msg = new K2.OSC.Message(msg,a,b);
//       this.oscConnector.sendOSC(msg);
//     }
// 
//     this.getPresets = function(){
//       var keys = ["osc1-amplitude", "osc1-waveform", "osc1-finetune", "lfo1-freq", "lfo1-waveform", "lfo1-to-amplitude", "lfo1-to-freq", "osc2-amplitude", "osc2-waveform", "osc2-finetune", "lfo2-freq", "lfo2-waveform", "lfo2-to-amplitude", "lfo2-to-freq", "osc3-amplitude", "osc3-waveform", "osc3-finetune", "lfo3-freq", "lfo3-waveform", "lfo3-to-amplitude", "lfo3-to-freq", "noise-amplitude", "lp-freq", "lp-resonance", "lp-keyfollow", "lp-contour", "lp-envelope-a", "lp-envelope-d", "lp-envelope-s", "lp-envelope-r", "lp-lfo-freq", "lp-lfo-waveform", "lp-lfo-amplitude", "hp-freq", "hp-resonance", "hp-keyfollow", "hp-contour", "hp-envelope-a", "hp-envelope-d", "hp-envelope-s", "hp-envelope-r", "hp-lfo-freq"];
// 
//       var presets = new Array();
//       for(var i = 0; i<50; i++) {
//         var preset = {};
//         for(var j = 0; j<keys.length; j++) {
//           preset[keys[j]] = Math.random();
//         }
//         presets.push(preset);
//       }
//       return JSON.stringify(presets);
//     };
// 
//     this.print = function(message){
//       console.log(message);
//     }
// 
//     this.htmlDidLoad = function(){
//       main();
//     }
//   }
// 
//   ContainerApp = new Morphasynth.ContainerApp();
//   ContainerApp.init();
// }


/*
 * Main
 */
function showAudioError(error){
  console.log(error); 
  navigator.notification.alert('I can\'t start audio playback :(', null, 'Oh no!', 'Grrr!');
}

function main(){
  Keyboard = new Morphasynth.Keyboard();
  Keyboard.init();

  Options = new Morphasynth.Options();
  Options.init();

  Space = new Morphasynth.TimbreSpace();
  Space.init();
  PureData.configurePlayback(44100, 2, false, false, null, function(error){ 
    showAudioError(error);
  });

  PureData.openFile('./patches', '00_Morphasynth_NO_GUI.pd', null, function(error){ 
    showAudioError(error);
  });

  PureData.setActive(true, null, function(error){ 
    showAudioError(error);
  });

  setTimeout(function(){
    PureData.sendFloat(3, 'which-mfcc-x');
    PureData.sendFloat(10, 'which-mfcc-x');
    PureData.sendFloat(0.222222222222, 'cursor_width');
  }, 1000);
}

/*
 * Tell the container app that we're ready
 */
document.addEventListener('deviceready', main, false);

// $(document).ready(function(){
  // if(typeof ContainerApp == "undefined"){
  //   createMockContainerApp();
  // }

  // ContainerApp.htmlDidLoad();
// });
