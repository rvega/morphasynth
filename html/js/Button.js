/*
 * Button Object
 */
Morphasynth.Button = function(){

  var synthCanvas = null;
  var name = null;
  var posX = 50;
  var posY = 50;
  var but;

  this.init = function (nombre, canvas){

    name = nombre;
    //console.log("nombrsito: "+name);
    synthCanvas = canvas;

    var self = this;

  };

  this.setPosition = function(px, py){
    posX = px;
    posY = py;
    //but.transform();
  }

  this.draw = function(px, py){
    //hexagon 375.5
    but = synthCanvas.path('M 373.9 433 L 127.1 433 L 2.5 216 L 127.1 1.4 L 373.9 1.4 L 498.5 216 L 373.9 433 L 373.9 433');
    but.attr({'stroke':'#488089','stroke-width':'1'});
    but.transform("s.1, T"+(px-250)+", "+(py-227)+"r90");
  }

}