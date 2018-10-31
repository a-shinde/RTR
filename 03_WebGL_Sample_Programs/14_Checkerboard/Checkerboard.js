// ----------------------
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros =
{
    VERTEX_ATTRIB: 0,
    COLOR_ATTRIB: 1,
    NORMAL_ATTRIB: 2,
    TEXTURE0_ATTRIB: 3
};

var vao_checkerboard;
var vbo_position;
var vbo_texture;
var textureCheckerboard=0;
var uniform_texture0_sampler;
var mvpUniform;

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var perspectiveProjectionMatrix;
var checkerImage;

function main()
{
	canvas = document.getElementById("ABS");
	if(!canvas)
		console.log("Couldn't get canvas!");
	else
		console.log("Canvas obtained");
	
	console.log("canvas Width: "+ canvas.width + "canvas Height: "+canvas.height);
	
	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	init();
	resize();
	draw();
}

function init()
{
    gl = canvas.getContext("webgl2");
    if(gl == null)
    {
        console.log("Couldn't find WebGL 2 context. Exiting");
        return;
    }

    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    //**********************************
    // Vertex Shader
    //**********************************
    var vertexShaderSourceCode =
                        "#version 300 es" +
                        "\n" +
                        "in vec4 vPosition;" +
                        "in vec2 vTexture0;" +
                        "out vec2 outTexture0;" +
                        "uniform mat4 u_mvp_matrix;" +
                        "void main(void)" +
                        "{" +
                        "gl_Position = u_mvp_matrix * vPosition;" +
                        "outTexture0 = vTexture0;" +
                        "}"


    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);
    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0)
            alert("Vertex Shader:" + error);
        uninitialize();
    }

    //**********************************
    // fragment Shader
    //**********************************

    var fragmentShaderSourceCode =
                        "#version 300 es" +
                        "\n" +
                        "precision highp float;" +
                        "in vec2 outTexture0;" +
                        "out vec4 FragColor;" +
                        "uniform sampler2D uTexture0Sampler;" +
                        "void main(void)" +
                        "{" +
                        "FragColor = texture(uTexture0Sampler, outTexture0);" +
                        "}"

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0)
            alert("Fragment Shader:" + error);
        uninitialize();
    }

    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VERTEX_ATTRIB, "vPosition");
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.TEXTURE0_ATTRIB, "vTexture0");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");
    uniform_texture0_sampler = gl.getUniformLocation(shaderProgramObject, "uTexture0Sampler");


    //-----------------------------------------
    //    Vao Checkboard

    vao_checkerboard = gl.createVertexArray();
    gl.bindVertexArray(vao_checkerboard);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, 0, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VERTEX_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.VERTEX_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    var checkerboardTexCord = new Float32Array([1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0]);

    vbo_texture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
    gl.bufferData(gl.ARRAY_BUFFER, checkerboardTexCord, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.TEXTURE0_ATTRIB, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.TEXTURE0_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    makeCheckImage();

    textureSmiley = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, textureSmiley);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 64, 64, 0, gl.RGBA, gl.UNSIGNED_BYTE, checkerImage);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.bindTexture(gl.TEXTURE_2D, null);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
    gl.clearDepth(1.0);
    
    perspectiveProjectionMatrix = mat4.create();
    angleRotation = 0.0;
}

function makeCheckImage()
{
    var i, j, c;
    checkerImage = new Uint8Array(64 * 64 * 4);

    for (i = 0; i<64; i++)
    {
        for (j = 0; j<64; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            checkerImage[i * 256 + j * 4] = c;
            checkerImage[i*256 + j*4 + 1] = c;
            checkerImage[i*256 + j*4 + 2] = c;
            checkerImage[i * 256 + j * 4 + 3] = 255;
        }
    }
}


function resize()
{
    if (bFullscreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0, 0, canvas.width, canvas.height);

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width / canvas.height), 0.1, 100.0);
}

function toggleFullscreen()
{
    var fullscreen_element =
    document.fullscreenElement ||
    document.webkitFullscreenElement ||
    document.mozFullScreenElement ||
    document.msFullscreenElement ||
    null;

    if (fullscreen_element == null) {
        if (canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if (canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if (canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if (canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();
        bFullscreen = true;
    }
    else 
    {
        if (document.exitFullscreen)
            document.exitFullscreen();
        else if (document.mozCancelFullScreen)
            document.mozCancelFullScreen();
        else if (document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if (document.msExitFullscreen)
            document.msExitFullscreen();
        bFullscreen = false;
    }
}


function keyDown(event)
{
    switch (event.key)
    {
        case 'f':
        case 'F':
            toggleFullscreen();
            break;
    }

    switch (event.keyCode) {
        case 27:
            uninitialize();
            window.close();
            break;
    }
}

function mouseDown()
{
//	alert("Mouse is clicked");
}

function draw()
{
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    // Draw Pyramid
    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindTexture(gl.TEXTURE_2D, textureSmiley);
    gl.uniform1i(uniform_texture0_sampler, 0);

    var checkerBoardCord = new Float32Array
    ([
        0.0, 1.0, 0.0,
        -2.0, 1.0, 0.0,
        -2.0, -1.0, 0.0,
        0.0, -1.0, 0.0
    ]);

    var checkerBoardCordTiled = new Float32Array 
    ([
        2.41421, 1.0, -1.41421,
        1.0, 1.0, 0.0,
        1.0, -1.0, 0.0,
        2.41421, -1.0, -1.41421
    ])


    gl.bindVertexArray(vao_checkerboard);

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, checkerBoardCord, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, checkerBoardCordTiled, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);

    gl.useProgram(null)

    requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
    if (vao_checkerboard)
    {
        gl.deleteVertexArray(vao_checkerboard);
        vao_checkerboard = null;
    }

    if (vbo_position)
    {
        gl.deleteBuffer(vbo_position);
        vbo_position = null;
    }

    if (vbo_texture) {
        gl.deleteBuffer(vbo_texture);
        vbo_texture = null;
    }

    if (shaderProgramObject)
    {
        if (fragmentShaderObject)
        {
            gl.detachShader(shaderProgramObject, fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject = null;
        }

        if (vertexShaderObject)
        {
            gl.detachShader(shaderProgramObject, vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject = null;
        }

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}
