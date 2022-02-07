export type VertexShaderSource = string;
export type FragmentShaderSource = string;


export function getWebGL2Ctx(canvasId: string): WebGL2RenderingContext {
    const canvas = document.getElementById(canvasId) as HTMLCanvasElement | null;
    const gl = canvas?.getContext("webgl2");
    if (!gl) {
        throw new Error('canvas not found or no WebGL2 compatibility on this browser.');
    }
    return gl;
}


export function makeShader(gl: WebGL2RenderingContext, source: VertexShaderSource | FragmentShaderSource, type: number): WebGLShader {
    const shader = gl.createShader(type);
    if (!shader) {
        throw new Error(`gl.createShader() failed.`);
    }
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (gl.getShaderParameter(shader, gl.COMPILE_STATUS)) { // compiled
        return shader;
    } else {
        const error = gl.getShaderInfoLog(shader);
        gl.deleteShader(shader);
        throw new Error(error ?? 'failed to create shader.');
    }
}


export function makeProgram(gl: WebGL2RenderingContext, vss: VertexShaderSource, fss: FragmentShaderSource): WebGLProgram {
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
    } else {
        const error = gl.getProgramInfoLog(program);
        gl.deleteProgram(program);
        throw new Error(error ?? 'failed to create program.');
    }
}


export function resizeCanvasToDisplay(canvas: HTMLCanvasElement): void {
    canvas.height = canvas.clientHeight;
    canvas.width = canvas.clientWidth;
}