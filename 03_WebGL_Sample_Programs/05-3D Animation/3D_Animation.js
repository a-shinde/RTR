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

var vao_pyramid;
var vao_cube;
var vbo_position;
var vbo_color;
var mvpUniform;

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var perspectiveProjectionMatrix;

var angleRotation;

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
                "in vec4 vColor;" +
                "out vec4 outColor;" +
                "uniform mat4 u_mvp_matrix;" +
                "void main(void)" +
                "{" +
                "gl_Position = u_mvp_matrix * vPosition;" +
                "outColor = vColor;" +
                "}";
                        

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
                        "in vec4 outColor;" +
                        "out vec4 FragColor;" +
                        "void main(void)" +
                        "{" +
                        "FragColor = outColor;" +
                        "}";

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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.COLOR_ATTRIB, "vColor");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");


    //-----------------------------------------
    //    Vao Pyramid

    var pyramidVertices = new Float32Array
        ([
                        0.0, 1.0, 0.0,
                        -1.0, -1.0, 1.0,
                        1.0, -1.0, 1.0,
                      //Right Face
                        0.0, 1.0, 0.0,
                        1.0, -1.0, 1.0,
                        1.0, -1.0, -1.0,
                     //Back Face
                        0.0, 1.0, 0.0,
                        1.0, -1.0, -1.0,
                        -1.0, -1.0, -1.0,
                     //Left Face
                        0.0, 1.0, 0.0,
                        -1.0, -1.0, -1.0,
                        -1.0, -1.0, 1.0
        ]);

    var pyramidColors = new Float32Array
        ([
                        1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0,
                     //Right Face
                        1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0,
                        0.0, 1.0, 0.0,
                    //Back Face
                        1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0,
                    //Left Face
                        1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0,
                        0.0, 1.0, 0.0
        ]);


    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VERTEX_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.VERTEX_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_color = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.COLOR_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.COLOR_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    //-----------------------------------------
    //    Vao Cube

    var cubeVertices = new Float32Array
        ([
                        1.0, 1.0, 1.0,
                        -1.0, 1.0, 1.0,
                        -1.0, -1.0, 1.0,
                        1.0, -1.0, 1.0,
                    //Right Face
                        1.0, 1.0, -1.0,
                        1.0, 1.0, 1.0,
                        1.0, -1.0, 1.0,
                        1.0, -1.0, -1.0,
                    //Back Face
                        -1.0, 1.0, -1.0,
                        1.0, 1.0, -1.0,
                        1.0, -1.0, -1.0,
                        -1.0, -1.0, -1.0,
                    //Left Face
                        -1.0, 1.0, 1.0,
                        -1.0, 1.0, -1.0,
                        -1.0, -1.0, -1.0,
                        -1.0, -1.0, 1.0,
                    //Top face
                        1.0, 1.0, -1.0,
                        -1.0, 1.0, -1.0,
                        -1.0, 1.0, 1.0,
                        1.0, 1.0, 1.0,
                    //Bottom face
                        1.0, -1.0, 1.0,
                        -1.0, -1.0, 1.0,
                        -1.0, -1.0, -1.0,
                        1.0, -1.0, -1.0
    ]);

    var cubeColors = new Float32Array
    ([
                        //Front Face
                        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                       //Right Face
                        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                       //Back Face
                        0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                       //Left Face
                        1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                       //Top face
                        1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0,
                       //Bottom face
                        0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0

    ]);

    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VERTEX_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.VERTEX_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_color = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color);
    gl.bufferData(gl.ARRAY_BUFFER, cubeColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.COLOR_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.COLOR_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
    gl.clearDepth(1.0);

    perspectiveProjectionMatrix = mat4.create();
    angleRotation = 0.0;
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
    mat4.translate(modelViewMatrix, modelViewMatrix, [-1.7, 0.0, -5.5]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, angleRotation);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao_pyramid);
    gl.drawArrays(gl.TRIANGLES, 0, 12);
    gl.bindVertexArray(null);

    // Draw Cube
    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjectionMatrix);
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.7, 0.0, -5.5]);
    mat4.rotateX(modelViewMatrix, modelViewMatrix, angleRotation);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, angleRotation);
    mat4.rotateZ(modelViewMatrix, modelViewMatrix, angleRotation);
    mat4.scale(modelViewMatrix, modelViewMatrix, [0.75, 0.75, 0.75]);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao_cube);
    for(var i = 0; i < 6; i++)
        gl.drawArrays(gl.TRIANGLE_FAN, (i*4),4);
    gl.bindVertexArray(null);
    
    gl.useProgram(null)

    update();
    requestAnimationFrame(draw, canvas);
}

function update()
{
    if (angleRotation > 2*3.14)
        angleRotation = 0;

    angleRotation += 0.01;
}

function uninitialize()
{
    if (vao_pyramid)
    {
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid = null;
    }

    if (vao_cube)
    {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }

    if (vbo_position)
    {
        gl.deleteBuffer(vbo_position);
        vbo_position = null;
    }

    if (vbo_color) {
        gl.deleteBuffer(vbo_color);
        vbo_color = null;
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
