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

const LightingType =
{
    PER_VERTEX: 0,
    PER_FRAGMENT: 1
}

var lightingType = LightingType.PER_FRAGMENT;


var vao_pyramid;
var vbo_position;
var vbo_normal;

var model_matrix_uniform;
var view_matrix_uniform;
var projection_uniform;

var LKeyPressed_uniform;
var La_Uniform;
var Ld_Uniform;
var Ls_Uniform;
var light_Position_uniform;

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

var light_ambient;
var light_diffuse;
var light_specular;
var light_position;

var material_ambient = [0.0, 0.0, 0.0];
var material_diffuse = [1.0, 1.0, 1.0];
var material_specular = [1.0, 1.0, 1.0];
var material_shininess = 50.0;

function main() {
    canvas = document.getElementById("ABS");
    if (!canvas)
        console.log("Couldn't get canvas!");
    else
        console.log("Canvas obtained");

    console.log("canvas Width: " + canvas.width + "canvas Height: " + canvas.height);

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    init();
    resize();
    draw();
}

function init() {
    gl = canvas.getContext("webgl2");
    if (gl == null) {
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
                "uniform mediump int u_lighting_type;" +
                "uniform mat3 u_La;" +
                "uniform mat3 u_Ld;" +
                "uniform mat3 u_Ls;" +
                "uniform mat4 u_light_position;" +
                "uniform vec3 u_Ka;" +
                "uniform vec3 u_Kd;" +
                "uniform vec3 u_Ks;" +
                "uniform float u_material_shininess;" +

                "out vec3 out_color;" +
                "out vec3 transformed_normals;" +
                "out mat3 light_direction;" +
                "out vec3 viewer_vector;" +

                "void main(void)" +
                "{" +
                "   if(u_lighting_enabled == 1)" +
                "   {" +
                "       for(int index = 0; index < 2; index++)" +
                "       {" +
                "           vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
                "           transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" +
                "           light_direction[index] = vec3(u_light_position[index]) - eye_coordinates.xyz;" +
                "           viewer_vector = -eye_coordinates.xyz;" +
                "           if(u_lighting_type == 0)" +
                "           {" +
                "               transformed_normals = normalize(transformed_normals);" +
                "               light_direction[index] = normalize(light_direction[index]);" +
                "               viewer_vector = normalize(viewer_vector);" +
                "               float tn_dot_ld = max(dot(transformed_normals, light_direction[index]),0.0);" +
                "               vec3 ambient = u_La[index] * u_Ka;" +
                "               vec3 diffuse = u_Ld[index] * u_Kd * tn_dot_ld;" +
                "               vec3 reflection_vector = reflect(-light_direction[index], transformed_normals);" +
                "               vec3 viewer_vector1 = normalize(-eye_coordinates.xyz);" +
                "               vec3 specular = u_Ls[index] * u_Ks * pow(max(dot(reflection_vector, viewer_vector1), 0.0), u_material_shininess);" +
                "               out_color = out_color + ambient + diffuse + specular;" +
                "           }" +
                "       }" +
                "   }" +
                "   gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
                "}";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);
    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
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
                "in mat3 light_direction;" +
                "in vec3 viewer_vector;" +

                "uniform mat3 u_La;" +
                "uniform mat3 u_Ld;" +
                "uniform mat3 u_Ls;" +
                "uniform vec3 u_Ka;" +
                "uniform vec3 u_Kd;" +
                "uniform vec3 u_Ks;" +
                "uniform float u_material_shininess;" +
                "uniform int u_lighting_enabled;" +
                "uniform int u_lighting_type;" +

                "out vec4 FragColor;" +

                "void main(void)" +
                "{" +
                "   vec3 phong_ads_color;" +
                "   if(u_lighting_enabled == 1)" +
                "   {" +
                "       if (u_lighting_type == 1)" +
                "       {" +
                "           vec3 normalized_transformed_normals=normalize(transformed_normals);" +
                "           vec3 normalized_viewer_vector=normalize(viewer_vector);" +
                "           for(int index = 0; index < 2; index++)" +
                "           {" +
                "               vec3 normalized_light_direction=normalize(light_direction[index]);" +
                "               vec3 ambient = u_La[index] * u_Ka;" +
                "               float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" +
                "               vec3 diffuse = u_Ld[index] * u_Kd * tn_dot_ld;" +
                "               vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" +
                "               vec3 specular = u_Ls[index] * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" +
                "               phong_ads_color= phong_ads_color + ambient + diffuse + specular;" +
                "           }" +
                "       }" +
                "       else" +
                "       {" +
                "           phong_ads_color = out_color;" +
                "       }" +
                "   }" +
                "   else" +
                "   {" +
                "       phong_ads_color = vec3(1.0, 1.0, 1.0);" +
                "   }" +
                "   phong_ads_color = min(phong_ads_color, vec3(1.0)) ;" +
                "   FragColor = vec4(phong_ads_color, 1.0);" +
                "}";

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0)
            alert("Fragment Shader:" + error);
        uninitialize();
    }

    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VERTEX_ATTRIB, "vPosition");
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.NORMAL_ATTRIB, "vNormal");

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


    //-----------------------------------------
    //    Vao Cube

    var pyramidVertices = new Float32Array
        ([
                                      //Front Face
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

    var pyramidNormals = new Float32Array
    ([
                 0.0, 0.447214, 0.894427,
                    0.0, 0.447214, 0.894427,
                    0.0, 0.447214, 0.894427,

                    0.894427, 0.447214, 0.0,
                    0.894427, 0.447214, 0.0,
                    0.894427, 0.447214, 0.0,

                    0.0, 0.447214, -0.894427,
                    0.0, 0.447214, -0.894427,
                    0.0, 0.447214, -0.894427,

                    -0.894427, 0.447214, 0.0,
                    -0.894427, 0.447214, 0.0,
                    -0.894427, 0.447214, 0.0,

    ]);

    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VERTEX_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.VERTEX_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_normal = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.NORMAL_ATTRIB, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.NORMAL_ATTRIB);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
    gl.clearDepth(1.0);

    perspectiveProjectionMatrix = mat4.create();


    light_ambient = mat3.fromValues(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    light_diffuse = mat3.fromValues(1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    light_specular = mat3.fromValues(1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    light_position = mat4.fromValues(2.0, 1.0, 1.0, 1.0, -2.0, 1.0, 1.0, 1.0);

    angleRotation = 0.0;
}

function resize() {
    if (bFullscreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0, 0, canvas.width, canvas.height);

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width / canvas.height), 0.1, 100.0);
}

function toggleFullscreen() {
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
    else {
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


function keyDown(event) {
    switch (event.key) {
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
    }

    switch (event.keyCode) {
        case 27:
            uninitialize();
            window.close();
            break;
    }
}

function mouseDown() {
    //	alert("Mouse is clicked");
}

function draw() {
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    if (isLight) {
        gl.uniform1i(LKeyPressed_uniform, 1);

        gl.uniformMatrix3fv(La_Uniform, false, light_ambient);
        gl.uniformMatrix3fv(Ld_Uniform, false, light_diffuse);
        gl.uniformMatrix3fv(Ls_Uniform, false, light_specular);
        gl.uniformMatrix4fv(lightPosition_uniform, false, light_position);
        gl.uniform1i(lightingType_uniform, lightingType);

        gl.uniform3fv(Ka_Uniform, material_ambient);
        gl.uniform3fv(Kd_Uniform, material_diffuse);
        gl.uniform3fv(Ks_Uniform, material_specular);
        gl.uniform1f(Kshine_Uniform, material_shininess);
    }
    else {
        gl.uniform1i(LKeyPressed_uniform, 0);
    }

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(modelMatrix, modelMatrix, angleRotation);

    gl.uniformMatrix4fv(model_matrix_uniform, false, modelMatrix);
    gl.uniformMatrix4fv(view_matrix_uniform, false, viewMatrix);
    gl.uniformMatrix4fv(projection_uniform, false, perspectiveProjectionMatrix);

    gl.bindVertexArray(vao_pyramid);
    gl.drawArrays(gl.TRIANGLES, 0, 12);
    gl.bindVertexArray(null);

    gl.useProgram(null)

    update();
    requestAnimationFrame(draw, canvas);
}

function update() {
    if (angleRotation > 2 * 3.14)
        angleRotation = 0;

    if (isAnimation)
        angleRotation += 0.005;
}

function uninitialize() {
    if (vao_pyramid) {
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid = null;
    }
    if (vbo_position) {
        gl.deleteBuffer(vbo_position);
        vbo_position = null;
    }

    if (vbo_normal) {
        gl.deleteBuffer(vbo_normal);
        vbo_normal = null;
    }

    if (shaderProgramObject) {
        if (fragmentShaderObject) {
            gl.detachShader(shaderProgramObject, fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject = null;
        }

        if (vertexShaderObject) {
            gl.detachShader(shaderProgramObject, vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject = null;
        }

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}
