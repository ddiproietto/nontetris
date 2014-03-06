var Module = {
preRun: [],
postRun: [],
print: function() {
	var text = Array.prototype.slice.call(arguments).join(' ');
	console.log(text);
},
printErr: function() {
	var text = Array.prototype.slice.call(arguments).join(' ');
	console.error(text);
},
canvas: null,
setStatus: function(text) {
},
totalDependencies: 0,
monitorRunDependencies: function(left) {
  this.totalDependencies = Math.max(this.totalDependencies, left);
}
};

if (document.readyState != "loading") {
	Module.canvas = document.getElementById('glcanvas');	
} else {
	document.addEventListener("DOMContentLoaded", function(){
		Module.canvas = document.getElementById('glcanvas');	
	});
}
