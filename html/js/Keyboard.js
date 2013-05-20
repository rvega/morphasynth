if(typeof Morphasynth==='undefined') Morphasynth = {};

/*
 * Manages the piano keyboard
 */
Morphasynth.Keyboard = function(){

  var lastKey;

  this.init = function(){
    //calls the "mouse events" methods
    //the pressed
    $('.key').bind('mousedown touchstart', $.proxy(this.pressedKey, this));

    //the released
    $('.key').bind('mouseup touchend', $.proxy(this.releasedKey, this));
    $('#piano-scroll').scrollLeft( $("#piano").width()/2.56 );
  };

  this.pressedKey = function(event){
    var key = $(event.target);
    var octaveNumber = key.closest('.octave').attr('data-octave');
    var noteNumber = parseInt(octaveNumber*12) + parseInt(key.attr('data-note-number'));
    $(key).addClass("pressed");
    
    PureData.sendNoteOn(noteNumber, 64);

    // Debug:
    // console.log("MIDI note on: " + noteNumber);
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

    PureData.sendNoteOn(noteNumber, 0);

    // Debug:
    // console.log("MIDI note off: " + noteNumber);
  };
}
