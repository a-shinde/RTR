// ----------------------
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros =
{
    VDG_ATTRIBUTE_VERTEX:0,
    VDG_ATTRIBUTE_COLOR:1,
    VDG_ATTRIBUTE_NORMAL:2,
    VDG_ATTRIBUTE_TEXTURE0:3,
};

var model_matrix_uniform;
var view_matrix_uniform;
var projection_uniform;

var LKeyPressed_uniform;
var La_Uniform;
var Ld_Uniform;
var Ls_Uniform;
var lightPosition_uniform;

var Ka_Uniform;
var Kd_Uniform;
var Ks_Uniform;
var Kshine_Uniform;

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var perspectiveProjectionMatrix;

var isAnimation = false;
var isLight = false;

var angleRotation;

var light_ambient = [0.0, 0.0, 0.0];
var light_diffuse = [1.0, 1.0, 1.0];
var light_specular = [1.0, 1.0, 1.0];
var light_position = [100.0, 100.0, 100.0, 1.0];

var material_ambient = [0.0, 0.0, 0.0];
var material_diffuse = [1.0, 1.0, 1.0];
var material_specular = [1.0, 1.0, 1.0];
var material_shininess = 50.0;

var sphere = null;

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
                "in vec3 vNormal;" +

                "uniform mat4 u_model_matrix;" +
                "uniform mat4 u_view_matrix;" +
                "uniform mat4 u_projection_matrix;" +
                "uniform mediump int u_lighting_enabled;" +
                "uniform vec3 u_La;" +
                "uniform vec3 u_Ld;" +
                "uniform vec3 u_Ls;" +
                "uniform vec4 u_light_position;" +
                "uniform vec3 u_Ka;" +
                "uniform vec3 u_Kd;" +
                "uniform vec3 u_Ks;" +
                "uniform float u_material_shininess;" +

                "out vec3 phong_ads_color;" +

                "void main(void)" +
                "{" +
                "   if(u_lighting_enabled==1)" +
                "   {" +
                "       vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
                "       vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
                "       vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);" +
                "       float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" +
                "       vec3 ambient = u_La * u_Ka;" +
                "       vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
                "       vec3 reflection_vector = reflect(-light_direction, transformed_normals);" +
                "       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
                "       vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" +
                "       phong_ads_color=ambient + diffuse + specular;" +
                "   }" +
                "   else" +
                "   {" +
                "       phong_ads_color = vec3(1.0, 1.0, 1.0);" +
                "   }" +
                "   gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
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
                        "in vec3 phong_ads_color;" +
                        "out vec4 FragColor;" +

                        "void main(void)" +
                        "{" +
                        "   FragColor = vec4(phong_ads_color, 1.0);" +
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

    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    model_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
    view_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
    projection_uniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");

    LKeyPressed_uniform = gl.getUniformLocation(shaderProgramObject, "u_lighting_enabled");

    La_Uniform = gl.getUniformLocation(shaderProgramObject, "u_La");
    Ld_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ld");
    Ls_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ls");

    Ka_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
    Kd_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
    Ks_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");
    Kshine_Uniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

    lightPosition_uniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");;

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
   // gl.clearDepth(1.0);

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

        case 'l':
        case 'L':
            if (isLight == true)
                isLight = false;
            else
                isLight = true;
            break;

        case 'a':
        case 'A':
            if (isAnimation == true)
                isAnimation = false;
            else
                isAnimation = true;
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
    gl.clear(gl.COLOR_BUFFER_BIT );

    gl.useProgram(shaderProgramObject);

    if (isLight  == true)
    {
        gl.uniform1i(LKeyPressed_uniform, 1);
        gl.uniform3fv(La_Uniform, light_ambient);
        gl.uniform3fv(Ld_Uniform, light_diffuse);
        gl.uniform3fv(Ls_Uniform, light_specular);
        gl.uniform4fv(lightPosition_uniform, light_position);

        gl.uniform3fv(Ka_Uniform, material_ambient);
        gl.uniform3fv(Kd_Uniform, material_diffuse);
        gl.uniform3fv(Ks_Uniform, material_specular);
        gl.uniform1f(Kshine_Uniform, material_shininess);
    }
    else
    {
        gl.uniform1i(LKeyPressed_uniform, 0);
    }
    
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(model_matrix_uniform, false, modelMatrix);
    gl.uniformMatrix4fv(view_matrix_uniform, false, viewMatrix);
    gl.uniformMatrix4fv(projection_uniform, false, perspectiveProjectionMatrix);


    sphere.draw();
    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
    if (sphere)
    {
        sphere.deallocate();
        sphere = null;
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
