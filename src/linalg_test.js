import * as la from './linalg.js'; 

var test = la.convert_contiguous([1,1,1, 4,3,-1,3,5,3], [3,3]);
var ident = la.identity(3, 3)
var test2 = la.inverse(test, ident)
