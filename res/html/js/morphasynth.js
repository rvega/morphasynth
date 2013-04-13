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

   this.init = function(){
      // Load presets
      this.presets = JSON.parse(ContainerApp.getPresets());

      // Bind mouse events
      self = this;
      $('#timbre-space').bind('doubleclick', function(event){
         if(event.which == 3){ // Right click
            self.rightDoubleClick.call(self);
         }
         else{
            self.leftDoubleClick.call(self);
         }
      });
      $('#timbre-space').bind('click', function(event){
         if(event.which == 3){
            self.rightClick.call(self);
         }
         else{
            self.leftClick.call(self);
         }
      });

      // Create drawing surface
      var ts = $('#timbre-space');
      this.width = ts.width();
      this.height = ts.height();
      this.canvas = Raphael('timbre-space', ts.width(), ts.height());

      // Draw the points
      this.drawPoints();
   };

   this.drawPoints = function(){
      var preset, x, y, xDescriptor, yDescriptor;
      for(var i=0; i<this.presets.length; i++) {
         preset = this.presets[i];
         xDescriptor = Options.x;
         yDescriptor = Options.y;
         x = preset[xDescriptor] * this.width;
         y = preset[yDescriptor] * this.height;
         this.canvas.circle(x, y, 5);
      }
   };

   this.leftDoubleClick = function(event){
      console.log('a');
   }

   this.leftClick = function(event){
      console.log('b');
   }
   
   this.rightDoubleClick = function(event){
      console.log('c');
   }

   this.rightClick = function(event){
      console.log('d');
   }
}

/*
 * Mock class, simulates the C++ app that contains the webkit instance where this script runs
 * If ran within the C++ app, this object will be provided to us
 */ 
function createMockContainerApp(){
   Morphasynth.ContainerApp = function(){
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
