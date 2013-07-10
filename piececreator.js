var vertices = [
[ 0,0, 4,0, 4,1, 0,1 ], //I
	[ 0,0, 3,0, 3,2, 2,2, 2,1, 0,1 ], //J
	[ 0,0, 3,0, 3,1, 1,1, 1,2, 0,2], //L
	[ 0,0, 2,0, 2,2, 0,2 ], //O
	[ 1,0, 3,0, 3,1, 2,1, 2,2, 0,2, 0,1, 1,1], //S
	[ 0,0, 2,0, 2,1, 3,1, 3,2, 1,2, 1,1, 0,1], //Z
	[ 0,0, 3,0, 3,1, 2,1, 2,2, 1,2, 1,1, 0,1]  //T
	];

	var centers = [
{x:2  , y:0.5}, //I
{x:1.5, y:1  }, //J
{x:1.5, y:1  }, //L
{x:1  , y:1  }, //O
{x:1.5, y:1  }, //S
{x:1.5, y:1  }, //Z
{x:1.5, y:1  }, //T

];

for(var i = 0; i < vertices.length;i++)
{
	var p = "polygon <float> polI = { ";
	var pol = vertices[i];
	var c = centers[i];
	for(var j = 0; j <pol.length/2;j++)
	{
		p +="{ " ;
		p += (pol[j*2] - c.x).toString();
		p += ",";
		p += (pol[j*2+1] - c.y).toString();
		p += "}, ";
	}
	p = p.substr(0,p.length - 2);
	p += " };"
	print (p)
}
