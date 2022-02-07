/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ 593:
/***/ ((__unused_webpack_module, exports) => {


Object.defineProperty(exports, "__esModule", ({ value: true }));
exports.resizeCanvasToDisplay = exports.makeProgram = exports.makeShader = exports.getWebGL2Ctx = void 0;
function getWebGL2Ctx(canvasId) {
    const canvas = document.getElementById(canvasId);
    const gl = canvas === null || canvas === void 0 ? void 0 : canvas.getContext("webgl2");
    if (!gl) {
        throw new Error('canvas not found or no WebGL2 compatibility on this browser.');
    }
    return gl;
}
exports.getWebGL2Ctx = getWebGL2Ctx;
function makeShader(gl, source, type) {
    const shader = gl.createShader(type);
    if (!shader) {
        throw new Error(`gl.createShader() failed.`);
    }
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (gl.getShaderParameter(shader, gl.COMPILE_STATUS)) { // compiled
        return shader;
    }
    else {
        const error = gl.getShaderInfoLog(shader);
        gl.deleteShader(shader);
        throw new Error(error !== null && error !== void 0 ? error : 'failed to create shader.');
    }
}
exports.makeShader = makeShader;
function makeProgram(gl, vss, fss) {
    const program = gl.createProgram();
    if (!program) {
        throw new Error(`gl.createProgram() failed.`);
    }
    const vssShader = makeShader(gl, vss, gl.VERTEX_SHADER);
    const fssShader = makeShader(gl, fss, gl.FRAGMENT_SHADER);
    gl.attachShader(program, vssShader);
    gl.attachShader(program, fssShader);
    gl.linkProgram(program);
    if (gl.getProgramParameter(program, gl.LINK_STATUS)) {
        return program;
    }
    else {
        const error = gl.getProgramInfoLog(program);
        gl.deleteProgram(program);
        throw new Error(error !== null && error !== void 0 ? error : 'failed to create program.');
    }
}
exports.makeProgram = makeProgram;
function resizeCanvasToDisplay(canvas) {
    canvas.height = canvas.clientHeight;
    canvas.width = canvas.clientWidth;
}
exports.resizeCanvasToDisplay = resizeCanvasToDisplay;


/***/ })

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId](module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
var __webpack_exports__ = {};
// This entry need to be wrapped in an IIFE because it need to be isolated against other modules in the chunk.
(() => {
var exports = __webpack_exports__;
var __webpack_unused_export__;

__webpack_unused_export__ = ({ value: true });
const utils_1 = __webpack_require__(593);
const vss = `#version 300 es

in vec2 a_position;
in vec2 a_texCoord;
uniform vec2 u_resolution;
out vec2 v_texCoord;

vec4 pixel_to_clip(vec2 position, vec2 resolution) {
    vec2 clip = ((position / resolution) * 2.0 - 1.0) * vec2(1, -1);
    return vec4(clip, 0, 1);
}

void main() {

    gl_Position = pixel_to_clip(a_position, u_resolution);
    // pase al fss
    v_texCoord = a_texCoord;
}
`;
const fss = `#version 300 es

precision highp float;
uniform sampler2D u_image;
in vec2 v_texCoord;
out vec4 outColor;

void main() {

    outColor = texture(u_image, v_texCoord);
}
`;
const image = new Image();
image.src = "../src/tilemap.png"; //"https://webgl2fundamentals.org/webgl/resources/leaves.jpg";
image.crossOrigin = "anonymous";
image.onload = () => render(image);
function render(image) {
    const gl = (0, utils_1.getWebGL2Ctx)('main-canvas');
    const program = (0, utils_1.makeProgram)(gl, vss, fss);
    const a_position_loc = gl.getAttribLocation(program, "a_position");
    const a_texCoord_loc = gl.getAttribLocation(program, "a_texCoord");
    const u_resolution_loc = gl.getUniformLocation(program, "u_resolution");
    const u_image_loc = gl.getUniformLocation(program, "u_image");
    // Create a vertex array object (attribute state)
    const vao = gl.createVertexArray();
    // make it the one we work on
    gl.bindVertexArray(vao);
    // buffer position
    const position_buffer = gl.createBuffer();
    // Turn on the attribute
    gl.enableVertexAttribArray(a_position_loc);
    // Bind it to ARRAY_BUFFER (think of it as ARRAY_BUFFER = positionBuffer)
    gl.bindBuffer(gl.ARRAY_BUFFER, position_buffer);
    // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    gl.vertexAttribPointer(a_position_loc, 2, // size
    gl.FLOAT, // type
    false, // normalize
    0, // stride (0 = move forward size * sizeof(type) each iteration to get the next position)
    0); // offset
    // provide texture coordinates for the rectangle.
    const texCoord_buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, texCoord_buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
    ]), gl.STATIC_DRAW);
    // Turn on the attribute
    gl.enableVertexAttribArray(a_texCoord_loc);
    // Tell the attribute how to get data out of texCoordBuffer (ARRAY_BUFFER)
    var size = 2; // 2 components per iteration
    var type = gl.FLOAT; // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0; // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0; // start at the beginning of the buffer
    gl.vertexAttribPointer(a_texCoord_loc, size, type, normalize, stride, offset);
    // Create a texture.
    var texture = gl.createTexture();
    // make unit 0 the active texture uint
    // (ie, the unit all other texture commands will affect
    gl.activeTexture(gl.TEXTURE0 + 0);
    // Bind it to texture unit 0' 2D bind point
    gl.bindTexture(gl.TEXTURE_2D, texture);
    // Set the parameters so we don't need mips and so we're not filtering
    // and we don't repeat at the edges
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    // Upload the image into the texture.
    var mipLevel = 0; // the largest mip
    var internalFormat = gl.RGBA; // format we want in the texture
    var srcFormat = gl.RGBA; // format of data we are supplying
    var srcType = gl.UNSIGNED_BYTE; // type of data we are supplying
    gl.texImage2D(gl.TEXTURE_2D, mipLevel, internalFormat, srcFormat, srcType, image);
    (0, utils_1.resizeCanvasToDisplay)(gl.canvas);
    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    // Clear the canvas
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);
    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);
    // Pass in the canvas resolution so we can convert from
    // pixels to clipspace in the shader
    gl.uniform2f(u_resolution_loc, gl.canvas.width, gl.canvas.height);
    // Tell the shader to get the texture from texture unit 0
    gl.uniform1i(u_image_loc, 0);
    // Bind the position buffer so gl.bufferData that will be called
    // in setRectangle puts data in the position buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, position_buffer);
    // Set a rectangle the same size as the image.
    setRectangle(gl, 0, 0, image.width, image.height);
    // Draw the rectangle.
    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 6;
    gl.drawArrays(primitiveType, offset, count);
}
function setRectangle(gl, x, y, width, height) {
    var x1 = x;
    var x2 = x + width;
    var y1 = y;
    var y2 = y + height;
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        x1, y1,
        x2, y1,
        x1, y2,
        x1, y2,
        x2, y1,
        x2, y2,
    ]), gl.STATIC_DRAW);
}

})();

/******/ })()
;