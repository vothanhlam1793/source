// FontPixels.js by TheGreatRambler https://github.com/TheGreatRambler/fontpixels.js/
// bin : pixel data, ex:
//       000001001000000000000010000000000000000000000000000000000100001000010000000000000000000000000
//       000011001000000000000010000111111100000000000000000000000100001000010000000111111111111000000
//       000010011111111100000110000000001100000000000000000000000100001000010000000100001100001000000
//       000110011010111100011111100000001100000000000000000000000100001000010000000110001100001000000
//       000100010000000100011111110000011000000000000000000000011111111111111110000111111111111000000
//       001100110001001100000100100000110000000000000000000000011111111111111110000100001100001000000
//       011101100001000000001100100000110000000000000000000000000100001000010000000110001100001000000
//       010100001001001000001100100000110000000000001100000000000100001000010000000111111111111000000
//       010100001001001000001000101111111110000000001100000000000100001000010000000001100001100000000
//       000100011001001100001001100000110000000000000100000000000100001100010000000011100001110000000
//       000100010001000100001111000000110000000000000100000000000100001111110000001110100001111100000
//       000100010001000100000111000000110000000000001000000000000100000000000000011000100001000110000
//       000100110001000110000011100000110000000000000000000000000100000000000000000001100001000000000
//       000100100001000010000110110000110000000000000000000000000100000000000000000001100001000000000
//       000100000001000000001100010000110000000000000000000000000110000000000100000011000001000000000
//       000100001111000000011000000111100000000000000000000000000111111111111100001110000001000000000
// data: convert to byte, ex:
//       4, 128, 2, 0, 0, 0, 0, 66, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
//       12, 128, 2, 31, 192, 0, 0, 66, 16, 31, 254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
//       9, 255, 6, 0, 192, 0, 0, 66, 16, 16, 194, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
//       25, 175, 31, 128, 192, 0, 0, 66, 16, 24, 194, 0, 0, 0, 0, 0, 15, 1, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 128, 14, 0, 0, 0]
//       17, 1, 31, 193, 128, 0, 1, 255, 254, 31, 254, 0, 0, 7, 188, 0, 11, 1, 96, 0, 0, 0, 0, 0, 30, 120, 0, 0, 5, 128, 14, 6, 0, 0]
//       51, 19, 4, 131, 0, 0, 1, 255, 254, 16, 194, 0, 0, 7, 28, 0, 3, 0, 96, 0, 0, 0, 0, 0, 24, 56, 0, 0, 0, 128, 6, 6, 0, 0]
//       118, 16, 12, 131, 0, 0, 0, 66, 16, 24, 194, 0, 120, 2, 8, 60, 3, 0, 96, 30, 0, 0, 0, 0, 24, 16, 120, 49, 192, 128, 118, 6, 0, 0]
//       80, 146, 12, 131, 0, 12, 0, 66, 16, 31, 254, 0, 72, 2, 8, 127, 3, 0, 96, 127, 0, 0, 0, 255, 137, 145, 252, 127, 192, 128, 254, 6, 0, 0]
//       80, 146, 8, 191, 224, 12, 0, 66, 16, 6, 24, 0, 72, 3, 24, 195, 3, 0, 96, 225, 128, 0, 0, 0, 9, 147, 134, 28, 0, 129, 142, 6, 0, 0]
//       17, 147, 9, 131, 0, 4, 0, 67, 16, 14, 28, 0, 120, 3, 248, 193, 3, 0, 96, 192, 128, 0, 0, 0, 9, 147, 2, 24, 0, 131, 6, 6, 0, 0]
//       17, 17, 15, 3, 0, 4, 0, 67, 240, 58, 31, 0, 48, 2, 8, 255, 131, 0, 96, 192, 128, 0, 0, 255, 142, 83, 2, 24, 0, 131, 6, 6, 0, 0]
//       17, 17, 7, 3, 0, 8, 0, 64, 0, 98, 17, 128, 0, 2, 8, 192, 3, 0, 96, 192, 128, 0, 0, 0, 14, 115, 2, 24, 0, 131, 6, 0, 0, 0]
//       19, 17, 131, 131, 0, 0, 0, 64, 0, 6, 16, 0, 0, 2, 8, 192, 3, 0, 96, 193, 128, 0, 0, 0, 14, 99, 6, 24, 0, 129, 134, 6, 0, 0]
//       18, 16, 134, 195, 0, 0, 0, 64, 0, 6, 16, 0, 0, 7, 60, 231, 15, 225, 252, 115, 0, 0, 0, 0, 4, 97, 204, 127, 7, 241, 223, 15, 0, 0]
//       16, 16, 12, 67, 0, 0, 0, 96, 4, 12, 16, 0, 0, 7, 188, 62, 31, 227, 252, 62, 0, 0, 0, 0, 4, 32, 248, 127, 15, 240, 247, 6, 0, 0]
//       16, 240, 24, 30, 0, 0, 0, 127, 252, 56, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
//data2: data gruop within 16x16 matrix 

(function(root, factory) {
    if (typeof define === 'function' && define.amd) {
        define([], factory);
    } else if (typeof module === 'object' && module.exports) {
        module.exports = factory();
    } else {
        root.fontpixels = factory();
    }
}(typeof self !== 'undefined' ? self : this, function() {
    var fontpixels = {}
    fontpixels.generatepixels = function(txt, resolution, fontFamily, width, lines) {
        if (!resolution) {
            resolution = 30;
        }
        if (!fontFamily) {
            fontFamily = "Times New Roman";
        }
        if (!width) {
            width = 0.3;
        }
        var canvas = document.createElement("canvas");
        var context = canvas.getContext("2d");
        var i;
        var data32;
        var pixels = [];
        var bin = [[]];
        var fontstring = resolution + "px " + fontFamily;

        context.font = fontstring;
        canvas.width = 16 - (context.measureText(txt).width % 16 ) + context.measureText(txt).width ; // make sure it's 16x
        canvas.height = resolution;
        context.textBaseline = "top";
        context.font = fontstring;
        context.lineWidth = width;
        context.clearRect(0, 0, canvas.width, canvas.height);
        if (!lines || lines === false) {
            context.fillText(txt, 0, 0);
        } else {
            context.strokeText(txt, 0, 0);
        }

        data32 = new Uint32Array(context.getImageData(0, 2, canvas.width, canvas.height).data.buffer);
        for (i = 0; i < data32.length; i++) {
            var x = i % canvas.width;
            var y = Math.trunc(i / canvas.width);
            if (typeof bin[y] == 'undefined') bin[y] = [];

            if (data32[i] & 0x80000000) { // check alpha mask
                bin[y][x] = 1;
                pixels.push({
                    x: (i % canvas.width),
                    y: ((i / canvas.width) | 0)
                });
            }
            else
                bin[y][x] = 0;
        }

        var data = [];
        for (var y = 0; y < bin.length -2; y++)
        {
            if (typeof data[y] == 'undefined') data[y] = [];
            for (var x = 0; x < bin[0].length; x = x + 8)
            {
                data[y][Math.trunc(x/8)] = bin[y][x]*128+bin[y][x+1]*64+bin[y][x+2]*32+bin[y][x+3]*16+
                                           bin[y][x+4]*8+bin[y][x+5]*4+bin[y][x+6]*2+bin[y][x+7]
            }
        }

        var data2 = [];
        for (var x = 0; x < data[0].length; x=x+2)
        {
            var temp = [];
            for (var y = 0; y < 16 ; y ++)
            {
                temp.push(data[y][x]);
                temp.push(data[y][x+1]);
            }
            data2.push(temp);
        }
        reqSet(data2);
        return pixels;
    };
    return fontpixels;
}));


function reqSet(arr)
{  
    var charData = []
    for (var i = 0; i < arr[0].length; i++ ){
        for (var j = 0; j < arr.length; j++){
        charData[j*32+i] = arr[j][i];
        }
    } 

    var dataObj = { "len": charData.length, "data" : charData};
    var dataStr = JSON.stringify(dataObj);

    var xhr = new XMLHttpRequest();
    xhr.open('POST', "/set", true);
    xhr.setRequestHeader('Content-Type', 'text/json');
    xhr.onload = function () {
        console.log(xhr.responseText);
        var users = JSON.parse(xhr.responseText);
        if (xhr.readyState == 4 && xhr.status == "200") {
            var res = JSON.parse(xhr.responseText);
            result = res.result;
            console.log(result);
        } else {
            console.error(users);
        }
    }

    xhr.send(dataStr);
    console.log(dataStr);

    /* //  因為無法解析ascii code為0，暫時停止開發
    console.log(arr);
    // Serialize the data2 array
    var charData = []
    for (var i = 0; i < arr[0].length; i++ ){
        for (var j = 0; j < arr.length; j++){
        charData[j*32+i] = arr[j][i];
        }
    } 
    // transform into u8array, because XMLHttpRequest only send ascii by u8array
    var data = Uint8Array.from(charData);
    var xhr = new XMLHttpRequest();
    xhr.open('POST', "http://172.20.10.9/set", true);
    //xhr.setRequestHeader('Content-Type', 'text/json'); // because arudino (server.hasArg("plain")== false)
    xhr.onload = function () {
        console.log(xhr.responseText);
        var users = JSON.parse(xhr.responseText);
        if (xhr.readyState == 4 && xhr.status == "200") {
            var res = JSON.parse(xhr.responseText);
            result = res.result;
            console.log(result);
        } else {
            console.error(users);
        }
    }

    xhr.send(data.buffer);
    console.log(data);

    //document.write(String.fromCharCode.apply(String, arr[0]));
    */
} 

