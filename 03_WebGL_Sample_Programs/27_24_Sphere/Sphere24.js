// ----------------------
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros =
{
    VDG_ATTRIBUTE_VERTEX: 0,
    VDG_ATTRIBUTE_COLOR: 1,
    VDG_ATTRIBUTE_NORMAL: 2,
    VDG_ATTRIBUTE_TEXTURE0: 3,
};

const LightingType =
{
    PER_VERTEX: 0,
    PER_FRAGMENT:1
}

var lightingType = LightingType.PER_FRAGMENT;

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

var isAnimation = true;
var isLight = false;

var angleRotation = 1.4;

var light_ambient = [0.0, 0.0, 0.0];
var light_diffuse = [1.0, 1.0, 1.0];
var light_specular = [1.0, 1.0, 1.0];
var light_position = [0.0, 0.0, 0.0, 1.0];

var material_ambient = [0.0, 0.0, 0.0];
var material_diffuse = [1.0, 1.0, 1.0];
var material_specular = [1.0, 1.0, 1.0];
var material_shininess = 50.0;

var sphere = null;
var rotationAxis = 0;
var radius = 50.0;

var materials = new Float32Array ([ 0.0215, 0.1745, 0.0215, 1.0, 0.07568, 0.61424, 0.07568, 1.0, 0.633, 0.727811, 0.633, 1.0, 0.6 * 128.0,
    0.135, 0.2225, 0.1575, 1.0, 0.54, 0.89, 0.63, 1.0, 0.316228, 0.316228, 0.316228, 1.0, 0.1 * 128.0,
    0.05375, 0.05, 0.06625, 1.0, 0.18275, 0.17, 0.22525, 1.0, 0.332741, 0.328634, 0.346435, 1.0, 0.3 * 128.0,
    0.25, 0.20725, 0.20725, 1.0, 1.0, 0.829, 0.829, 1.0, 0.296648, 0.296648, 0.296648, 1.0, 0.088 * 128.0,
    0.1745, 0.01175, 0.01175, 1.0, 0.61424, 0.04136, 0.04136, 1.0, 0.727811, 0.626959, 0.626959, 1.0, 0.6 * 128.0,
    0.1, 0.18725, 0.1745, 1.0, 0.396, 0.74151, 0.69102, 1.0, 0.297254, 0.30829, 0.306678, 1.0, 0.1 * 128.0,
    0.329412, 0.223529, 0.027451, 1.0, 0.780392, 0.568627, 0.113725, 1.0, 0.992157, 0.941176, 0.807843, 1.0, 0.21794872 * 128.0,
    0.2125, 0.1275, 0.054, 1.0, 0.714, 0.4284, 0.18144, 1.0, 0.393548, 0.271906, 0.166721, 1.0, 0.2 * 128.0,
    0.25, 0.25, 0.25, 1.0, 0.4, 0.4, 0.4, 1.0, 0.774597, 0.774597, 0.774597, 1.0, 0.6 * 128.0,
    0.19125, 0.0735, 0.0225, 1.0, 0.7038, 0.27048, 0.0828, 1.0, 0.256777, 0.137622, 0.086014, 1.0, 0.1 * 128.0,
    0.24725, 0.1995, 0.0745, 1.0, 0.75164, 0.60648, 0.22648, 1.0, 0.628281, 0.555802, 0.366065, 1.0, 0.4 * 128.0,
    0.19225, 0.19225, 0.19225, 1.0, 0.50754, 0.50754, 0.50754, 1.0, 0.508273, 0.508273, 0.508273, 1.0, 0.4 * 128.0,
    0.0, 0.0, 0.0, 1.0, 0.01, 0.01, 0.01, 1.0, 0.50, 0.50, 0.50, 1.0, 0.25 * 128.0,
    0.0, 0.1, 0.06, 1.0, 0.0, 0.50980392, 0.50980392, 1.0, 0.50196078, 0.50196078, 0.50196078, 1.0, 0.25 * 128.0,
    0.0, 0.0, 0.0, 1.0, 0.1, 0.35, 0.1, 1.0, 0.45, 0.55, 0.45, 1.0, 0.25 * 128.0,
    0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 1.0, 0.7, 0.6, 0.6, 1.0, 0.25 * 128.0,
    0.0, 0.0, 0.0, 1.0, 0.55, 0.55, 0.55, 1.0, 0.70, 0.70, 0.70, 1.0, 0.25 * 128.0,
    0.0, 0.0, 0.0, 1.0, 0.5, 0.5, 0.0, 1.0, 0.60, 0.60, 0.50, 1.0, 0.25 * 128.0,
    0.02, 0.02, 0.02, 1.0, 0.01, 0.01, 0.01, 1.0, 0.4, 0.4, 0.4, 1.0, 0.078125 * 128.0,
    0.0, 0.05, 0.05, 1.0, 0.4, 0.5, 0.5, 1.0, 0.04, 0.7, 0.7, 1.0, 0.078125 * 128.0,
    0.0, 0.05, 0.0, 1.0, 0.4, 0.5, 0.4, 1.0, 0.04, 0.7, 0.04, 1.0, 0.078125 * 128.0,
    0.05, 0.0, 0.0, 1.0, 0.5, 0.4, 0.4, 1.0, 0.7, 0.04, 0.04, 1.0, 0.078125 * 128.0,
    0.05, 0.05, 0.05, 1.0, 0.5, 0.5, 0.5, 1.0, 0.7, 0.7, 0.7, 1.0, 0.078125 * 128.0,
    0.05, 0.05, 0.0, 1.0, 0.5, 0.5, 0.4, 1.0, 0.7, 0.7, 0.04, 1.0, 0.078125 * 128.0]);




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

    gl.clearColor(0.15, 0.15, 0.15, 1.0);

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
                "uniform mediump int u_lighting_type;" +
                "uniform vec3 u_La;" +
                "uniform vec3 u_Ld;" +
                "uniform vec3 u_Ls;" +
                "uniform vec4 u_light_position;" +
                "uniform vec3 u_Ka;" +
                "uniform vec3 u_Kd;" +
                "uniform vec3 u_Ks;" +
                "uniform float u_material_shininess;" +

                "out vec3 out_color;" +
                "out vec3 transformed_normals;" +
                "out vec3 light_direction;" +
                "out vec3 viewer_vector;" +

                "void main(void)" +
                "{" +
                "   if(u_lighting_enabled == 1)" +
                "   {" +
                "       vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
                "       transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" +
                "       light_direction = vec3(u_light_position) - eye_coordinates.xyz;" +
                "       viewer_vector = -eye_coordinates.xyz;" +

                "       if(u_lighting_type == 0)" +
                "       {" +
                "          transformed_normals = normalize(transformed_normals);" +
                "          light_direction = normalize(light_direction);" +
                "          viewer_vector = normalize(viewer_vector);" +
                "          float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" +
                "          vec3 ambient = u_La * u_Ka;" +
                "          vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
                "          vec3 reflection_vector = reflect(-light_direction, transformed_normals);" +
                "          vec3 viewer_vector1 = normalize(-eye_coordinates.xyz);" +
                "          vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector1), 0.0), u_material_shininess);" +
                "          out_color = ambient + diffuse + specular;" +
                "       }" +
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
                "in vec3 out_color;" +
                "in vec3 transformed_normals;" +
                "in vec3 light_direction;" +
                "in vec3 viewer_vector;" +

                "uniform vec3 u_La;" +
                "uniform vec3 u_Ld;" +
                "uniform vec3 u_Ls;" +
                "uniform vec3 u_Ka;" +
                "uniform vec3 u_Kd;" +
                "uniform vec3 u_Ks;" +
                "uniform float u_material_shininess;" +
                "uniform int u_lighting_enabled;" +
                "uniform int u_lighting_type;" +

                "out vec4 FragColor;" +

                "void main(void)" +
                "{" +
                "   vec3 phong_ads_color = out_color;" +
                "   if(u_lighting_enabled == 1)" +
                "   {" +
                "       if(u_lighting_type == 1)" +
                "       {" +
                "           vec3 normalized_transformed_normals=normalize(transformed_normals);" +
                "           vec3 normalized_light_direction=normalize(light_direction);" +
                "           vec3 normalized_viewer_vector=normalize(viewer_vector);" +
                "           vec3 ambient = u_La * u_Ka;" +
                "           float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" +
                "           vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
                "           vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" +
                "           vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" +
                "           phong_ads_color=ambient + diffuse + specular;" +
                "       }" +
                "   }" +
                "   else" +
                "   {" +
                "       phong_ads_color = vec3(1.0, 1.0, 1.0);" +
                "   }" +
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
    lightingType_uniform = gl.getUniformLocation(shaderProgramObject, "u_lighting_type");

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
        case 'e':
        case 'E':
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

        case 'f':
        case 'F':
            lightingType = LightingType.PER_FRAGMENT;
            break;

        case 'v':
        case 'V':
            lightingType = LightingType.PER_VERTEX;
            break;

        case 'x':
        case 'X':
            rotationAxis = 0;
            break;

        case 'y':
        case 'Y':
            rotationAxis = 1;
            break;

        case 'z':
        case 'Z':
            rotationAxis = 2;
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

    switch (rotationAxis)
    {
        case 0: // X Axis
            light_position[0] = 0;
            light_position[1] = radius * Math.cos(angleRotation);
            light_position[2] = radius * Math.sin(angleRotation);
            break;

        case 1: // Y Axis
            light_position[0] = radius * Math.cos(angleRotation);
            light_position[1] = 0;
            light_position[2] = radius * Math.sin(angleRotation);
            break;

        case 2: // Z Axis
            light_position[0] = radius * Math.cos(angleRotation);
            light_position[1] = radius * Math.sin(angleRotation);
            light_position[2] = -5.5;
            break;
    }

    if (isLight)
    {
        gl.uniform1i(LKeyPressed_uniform, 1);
        gl.uniform3fv(La_Uniform, light_ambient);
        gl.uniform3fv(Ld_Uniform, light_diffuse);
        gl.uniform3fv(Ls_Uniform, light_specular);
        gl.uniform4fv(lightPosition_uniform, light_position);
        gl.uniform1i(lightingType_uniform, lightingType);
    }
    else
    {
        gl.uniform1i(LKeyPressed_uniform, 0);
    }

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.5]);
    gl.uniformMatrix4fv(model_matrix_uniform, false, modelMatrix);

    gl.uniformMatrix4fv(view_matrix_uniform, false, viewMatrix);

    for (var row = 0; row < 6; row++)
    {
        for (var column = 0; column < 4; column++)
        {
            var material_ambient = new Float32Array ([ materials[(column * 6 + row )*13 + 0], materials[(column * 6 + row)*13 + 1], materials[(column * 6 + row)*13 + 2] ]);
            gl.uniform3fv(Ka_Uniform, material_ambient);

            var material_diffuse = new Float32Array ([ materials[(column * 6 + row)*13 + 4], materials[(column * 6 + row)*13 + 5], materials[(column * 6 + row)*13 + 6] ]);
            gl.uniform3fv(Kd_Uniform, material_diffuse);

            var material_specular = new Float32Array([materials[(column * 6 + row) * 13 + 8], materials[(column * 6 + row) * 13 + 9], materials[(column * 6 + row) * 13 + 10]]);
            gl.uniform3fv(Ks_Uniform, material_specular);

            gl.uniform1f(Kshine_Uniform, materials[(column * 6 + row) * 13 + 12]);

            gl.viewport(canvas.width * column / 4, (canvas.height * (5-row)/6), canvas.width / 4, canvas.height / 6);
            mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width / canvas.height * 6/4), 0.1, 100.0);
            gl.uniformMatrix4fv(projection_uniform, false, perspectiveProjectionMatrix)

            sphere.draw();
        }
    }

    gl.useProgram(null);
    update();
    requestAnimationFrame(draw, canvas);
}

function update()
{
    if (angleRotation > 2 * 3.14)
        angleRotation = 0;

    if (isAnimation)
        angleRotation += 0.01;
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
