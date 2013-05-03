/*
 * Global namespace for the app
 */
Morphasynth = {}

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
    }
    else{
      this.y = select.val();
    }
  }
}


/*
 * Manages the piano keyboard
 */
Morphasynth.Keyboard = function(){

  var lastKey;

  this.init = function(){
    //calls the "mouse events" methods
    //the pressed
    $('.key').bind('mousedown', $.proxy(this.pressedKey, this));

    //the released
    $('.key').bind('mouseup', $.proxy(this.releasedKey, this));
    $('#piano-scroll').scrollLeft( $("#piano").width()/2.56 );
  };

  this.pressedKey = function(event){
    var key = $(event.target);
    var octaveNumber = key.closest('.octave').attr('data-octave');
    var noteNumber = parseInt(octaveNumber*12) + parseInt(key.attr('data-note-number'));
    $(key).addClass("pressed");
    // Debug:
    console.log("MIDI note on: " + noteNumber);
    lastKey=key;
  };

  //Manages the released event on keyboard
  this.releasedKey = function(event){ 
    /*
       var key = $(event.target);
       var octaveNumber = key.closest('.octave').attr('data-octave');
       var noteNumber = parseInt(octaveNumber*12) + parseInt(key.attr('data-note-number'));
       */

    $(lastKey).removeClass("pressed");
    var octaveNumber = lastKey.closest('.octave').attr('data-octave');
    var noteNumber = parseInt(octaveNumber*12) + parseInt(lastKey.attr('data-note-number'));
    // Debug:
    console.log("MIDI note off: " + noteNumber);
  };

}

/*
 * Manages the 2D space
 */
Morphasynth.TimbreSpace = function(){

  this.presets = null;
  this.canvas = null;
  this.height = 0;
  this.width = 0;
  var pointer1;
  var pointer2;
  var pointer3;
  var pointer4;
  var bglines;
  var pointerX = $('#timbre-space').width()/2;
  var pointerY = $('#timbre-space').height()/2;
  var sendXY=false;

  this.init = function(){
    // Load presets
    this.presets = JSON.parse(ContainerApp.getPresets());

    // Bind mouse events
    self = this;

    // Create drawing surface
    var ts = $('#timbre-space');
    this.width = ts.width();
    this.height = ts.height();
    this.canvas = Raphael('timbre-space', ts.width(), ts.height());

    //set Background
    var bg = this.canvas.rect(0,0,ts.width(),ts.height());
    bg.attr("fill", "#002129");
    this.BackgroundBeauties(ts.width(),ts.height());

    //draw the pointer
    this.drawPointer();
    // Draw the points
    this.drawPoints();

    //iterTime controla el tiempo del intervalo de repetición del iterador intervalId
    interTime=1;
    var intervalId;

    //the pressed
    $('#timbre-space').bind('mousedown', $.proxy(this.pressedKey, this));

    //the released
    $('#timbre-space').bind('mouseup', $.proxy(this.releasedKey, this));

    $("#timbre-space").mousemove(function (event) {

      if(sendXY){
        pointerX = event.pageX;
        pointerY = event.pageY;

        self.sendPoses(event.pageX,event.pageY);
        self.actualicePointer();
        pointer.attr("cx",pointerX);
        pointer.attr("cy",pointerY);
      }
    }); 
  };

  this.pressedKey = function(event){
    console.log("pres");

    this.sendPoses(event.clientX,event.clientY);

    sendXY=true;
  };

  this.releasedKey = function(event){ 
    console.log("rels");
    sendXY=false;
  };

  //SEND CANVAS POSES
  this.sendPoses = function (xx, yy){
    console.log("pointer poses x:"+xx+" y:"+yy);
    ContainerApp.sendMessage("/pointer/",pointerX,pointerY);
  }

  this.drawPoints = function(){
    var preset, x, y, xDescriptor, yDescriptor;
    for(var i=0; i<this.presets.length; i++) {
      preset = this.presets[i];
      xDescriptor = Options.x;
      yDescriptor = Options.y;
      x = preset[xDescriptor] * this.width;
      y = preset[yDescriptor] * this.height-68;
      var cir = this.canvas.circle(x, y, 2);
      cir.attr("fill", "#FFF");
      cir.attr("stroke-width",0);
    }
  };

  this.drawPointer = function (){
    console.log("pointerX: "+pointerX+"  pointerY:"+pointerY);

    pointer1=this.canvas.circle(pointerX,pointerY,18);
    pointer1.attr("fill-opacity","0");
    pointer1.attr("stroke-width","1");
    pointer1.attr("stroke-opacity",".3");
    pointer1.attr("stroke","#fff");

    pointer2=this.canvas.circle(pointerX,pointerY,25);
    pointer2.attr("fill-opacity","0");
    pointer2.attr("stroke-width","2");
    pointer2.attr("stroke","#fff")

    pointer3=this.canvas.circle(pointerX,pointerY,33);
    pointer3.attr("fill-opacity","0");
    pointer3.attr("stroke-width","6");
    pointer3.attr("stroke","#fff");
    pointer3.attr("stroke-dasharray","--.");
    pointer3.attr("stroke-linecap","square");

//    pointer4=this.canvas.circle(pointerX,pointerY,33);
//    pointer4.attr("fill-opacity","0");
//    pointer4.attr("stroke-width","7");
//    pointer4.attr("stroke","#fff");


//    pointer4=this.canvas.path("M"+pointerX+","+(pointerY-33)+"c19,0,35,16,35,35S"+(pointerX+18)+","+(pointerY+35)+","+pointerX+","+(pointerY+33));
//    pointer4.attr("fill-opacity","0");
//    pointer4.attr("stroke-width","4");
//    pointer4.attr("stroke","#fff");
//    pointer4.attr("stroke-dasharray", "100 ");
  };

  this.actualicePointer = function (){
    pointer1.attr("cx",event.clientX);
    pointer1.attr("cy",event.clientY);
    pointer2.attr("cx",event.clientX);
    pointer2.attr("cy",event.clientY);
    pointer3.attr("cx",event.clientX);
    pointer3.attr("cy",event.clientY);
  };

  this.BackgroundBeauties = function (wid, hei){

    console.log("sisaasfajsdfiasdlfa"+wid);

    //Dots
    for(var i=15;i<wid;i+=30){
      for(var j=15;j<hei;j+=30){
        pointer=this.canvas.circle(i,j,.3);
        pointer.attr("fill","#fff");
        pointer.attr("stroke-width","0");
      }
    }

    //Under canvas Lines
    for(var i=0;i<(wid+100);i+=12){
      bglines=this.canvas.path("M"+i+",712L"+(i-100)+",780");
      bglines.attr("stroke","#fff");
      bglines.attr("stroke-width","1");
      bglines.attr("stroke-opacity",".3");
    }
  };
}

/*
 * Mock class, simulates the C++ app that contains the webkit instance where this script runs
 * If ran within the C++ app, this object will be provided to us
 */ 
function createMockContainerApp(){
  Morphasynth.ContainerApp = function(){
    // this.oscConnector = null;

    this.init = function(){
      this.proxyServer = {host: 'localhost', port: 1488};
      this.udpHosts = [{host: 'localhost', port: 9999}];
      this.handler = new K2.OSCHandler(this.proxyServer, this.udpHosts);
      this.oscConnector = this.handler.registerClient({
        clientID: "client1",
        oscCallback : function (message) {
          console.log ("client1 received message: ", message);
        }
      });

      var msg = new K2.OSC.Message('/hi/there/', 2, 3.0);
      this.oscConnector.sendOSC(msg);
    }

    this.sendMessage = function(msg, a ,b){
      var msg = new K2.OSC.Message(msg,a,b);
      this.oscConnector.sendOSC(msg);
    }

    this.getPresets = function(){
      var keys = ["osc1-amplitude", "osc1-waveform", "osc1-finetune", "lfo1-freq", "lfo1-waveform", "lfo1-to-amplitude", "lfo1-to-freq", "osc2-amplitude", "osc2-waveform", "osc2-finetune", "lfo2-freq", "lfo2-waveform", "lfo2-to-amplitude", "lfo2-to-freq", "osc3-amplitude", "osc3-waveform", "osc3-finetune", "lfo3-freq", "lfo3-waveform", "lfo3-to-amplitude", "lfo3-to-freq", "noise-amplitude", "lp-freq", "lp-resonance", "lp-keyfollow", "lp-contour", "lp-envelope-a", "lp-envelope-d", "lp-envelope-s", "lp-envelope-r", "lp-lfo-freq", "lp-lfo-waveform", "lp-lfo-amplitude", "hp-freq", "hp-resonance", "hp-keyfollow", "hp-contour", "hp-envelope-a", "hp-envelope-d", "hp-envelope-s", "hp-envelope-r", "hp-lfo-freq", "hp-lfo-waveform", "hp-lfo-amplitude", "envelope-a", "envelope-d", "envelope-s", "envelope-r", "mfcc-1", "mfcc-2", "mfcc-3", "mfcc-4", "mfcc-5", "mfcc-6", "mfcc-7", "mfcc-8", "mfcc-9", "mfcc-10", "mfcc-11", "mfcc-12", "mfcc-13"];

      var presets = new Array();
      for(var i = 0; i<50; i++) {
        var preset = {};
        for(var j = 0; j<keys.length; j++) {
          preset[keys[j]] = Math.random();
        }
        presets.push(preset);
      }
      return JSON.stringify(presets);
    };

    this.print = function(message){
      console.log(message);
    }

    this.htmlDidLoad = function(){
      main();
    }
  }

  ContainerApp = new Morphasynth.ContainerApp();
  ContainerApp.init();
}

/*
 * Main
 */
function main(){
  Keyboard = new Morphasynth.Keyboard();
  Keyboard.init();

  Options = new Morphasynth.Options();
  Options.init();

  Space = new Morphasynth.TimbreSpace();
  Space.init();
}

/*
 * Tell the container app that we're ready
 */
$(document).ready(function(){
  if(typeof ContainerApp == "undefined"){
    createMockContainerApp();
  }

  ContainerApp.htmlDidLoad();
});
