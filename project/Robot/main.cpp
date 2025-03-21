#include "main.h"
#include "TextureApp.h"
float timer=0;
vec3 camera = vec3(0,0,20);
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//以OpenGL version4.3版本為基準
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//是否向下相容,GLUT_FORWARD_COMPATIBLE不支援(?
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Sackboy");
	
	glewExperimental = GL_TRUE; //置於glewInit()之前
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	int ActionMenu,ModeMenu,ShaderMenu,ColorMenu;
	ActionMenu = glutCreateMenu(ActionMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("idle",0);
	glutAddMenuEntry("walk",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ColorMenu = glutCreateMenu(ColorMenuEvents);//建立右鍵菜單
												  //加入右鍵物件
	glutAddMenuEntry("init", 0);
	glutAddMenuEntry("texture", 1);
	glutAddMenuEntry("gray", 2);
	glutAddMenuEntry("circle", 3);
	glutAddMenuEntry("quantization", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ModeMenu = glutCreateMenu(ModeMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("Line",0);
	glutAddMenuEntry("Fill",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯


	glutCreateMenu(menuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddSubMenu("action",ActionMenu);
	glutAddSubMenu("mode",ModeMenu);
	glutAddSubMenu("color", ColorMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	glutMouseFunc(Mouse);
	glutTimerFunc (100, idle, 0); 
	glutMainLoop();
	return 0;
}
void ChangeSize(int w,int h){
	if(h == 0) h = 1;
	glViewport(0,0,w,h);
	Projection = perspective(80.0f,(float)w/h,0.1f,100.0f);
}
void Mouse(int button,int state,int x,int y){
	if(button == 2) isFrame = false;
}
void idle(int dummy){
	isFrame = true;
	int out = 0;
	if(action == WALK){
		updateObj(dummy);
		out = dummy+1;
		if(out > 24) out = 1;
	}
	else if(action == IDLE){
		resetObj(dummy);
		out = 0;
	}
	glutPostRedisplay();
	
	glutTimerFunc (42, idle, out); 
}
void resetObj(int f){
	for(int i = 0 ; i < PARTSNUM;i++){
		angles[i] = 0.0f;
	}	
}
void updateObj(int frame){
	switch(frame){
	case 0:
		//左手
		angles[2] = -0;
		//右手

		//腿
		angles[4] = 0;	
		
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		angles[2] +=2.5;
		angles[4] -=3.5;
		position += 0.05;
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		angles[2] -=2.5;
		angles[4] +=3.5;
		position -= 0.05;
		break;
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		angles[2] -=2.5;
		angles[4] +=3.5;
		position += 0.05;
		break;
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		angles[2] +=2.5;
		angles[4] -=3.5;
		position -= 0.05;
		break;
	}
}


 GLuint M_KaID;
 GLuint M_KdID;
 GLuint M_KsID;
 GLuint M_map_KdID;
 GLuint colormodID;
 GLuint timeID;
void init(){
	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//初始化角度陣列
		angles[i] = 0.0;
	colormod = 0;
	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "DSPhong_Material.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "DSPhong_Material.fp" },//fragment shader
		{ GL_NONE, NULL }};
	program = LoadShaders(shaders);//讀取shader

	glUseProgram(program);//uniform參數數值前必須先use shader
	
	colormodID = glGetUniformLocation(program, "ch");

	timeID = glGetUniformLocation(program, "time");

	MatricesIdx = glGetUniformBlockIndex(program,"MatVP");
	ModelID =  glGetUniformLocation(program,"Model");
    M_KaID = glGetUniformLocation(program,"Material.Ka");
	M_KdID = glGetUniformLocation(program,"Material.Kd");
	M_KsID = glGetUniformLocation(program,"Material.Ks");
	M_map_KdID = glGetUniformLocation(program, "Material.map_Kd");
	BodyTexLocation = glGetUniformLocation(program, "BodyTex");
	EyesTexLocation = glGetUniformLocation(program, "EyesTex");
	ZipTexLocation = glGetUniformLocation(program, "ZipTex");
	toungeTexLocation = glGetUniformLocation(program, "toungeTex");
	glUniform1i(BodyTexLocation, 0);
	glUniform1i(EyesTexLocation, 1);
	glUniform1i(ZipTexLocation, 2);
	glUniform1i(toungeTexLocation, 3);
	BodyTexHandle = TextureApp::GenTexture("Obj/Body.png");
	EyesTexHandle = TextureApp::GenTexture("Obj/Eyes.png");
	ZipTexHandle = TextureApp::GenTexture("Obj/Zip.png");
	toungeTexHandle = TextureApp::GenTexture("Obj/tounge.png");
	//or
	M_KdID = M_KaID+1;
	M_KsID = M_KaID+2;

	Projection = glm::perspective(80.0f,4.0f/3.0f,0.1f,100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	
	// Camera matrix
	View       = glm::lookAt(
		glm::vec3(0,10,25) , // Camera is at (0,10,25), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,1,0 to look upside-down)
		);

	Obj2Buffer();

	//UBO
	glGenBuffers(1,&UBO);
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(mat4)*2,NULL,GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);  
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER,0,UBO,0,UBOsize);
	glUniformBlockBinding(program, MatricesIdx,0);


	glClearColor(0.0,0.0,0.0,1);//black screen
}

#define DOR(angle) (angle*3.1415/180);
void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/************************************************************/
	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, BodyTexHandle);

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, EyesTexHandle);

	glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, ZipTexHandle);

	glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, toungeTexHandle);
	/************************************************************/

	glBindVertexArray(VAO);
	glUseProgram(program);//uniform參數數值前必須先use shader
	float eyey = DOR(eyeAngley);
	View       = lookAt(
		               vec3(eyedistance*sin(eyey),2,eyedistance*cos(eyey)) , // Camera is at (0,0,20), in World Space
		               vec3(0,0,0), // and looks at the origin
		               vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		                );
	updateModels();
	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(mat4),&View);
	glBufferSubData(GL_UNIFORM_BUFFER,sizeof(mat4),sizeof(mat4),&Projection);
	glBindBuffer(GL_UNIFORM_BUFFER,0);

	GLuint offset[3] = {0,0,0};//offset for vertices , uvs , normals
	for(int i = 0;i < PARTSNUM ;i++){
		glUniformMatrix4fv(ModelID,1,GL_FALSE,&Models[i][0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
							  3,				//vec3
							  GL_FLOAT,			//type
							  GL_FALSE,			//not normalized
							  0,				//strip
							  (void*)offset[0]);//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] +=  vertices_size[i]*sizeof(vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glVertexAttribPointer(1, 
							  2, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[1]);
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] +=  uvs_size[i]*sizeof(vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glVertexAttribPointer(2,
							  3, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[2]);
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] +=  normals_size[i]*sizeof(vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		string mtlname;//material name
		vec3 Ks = vec3(1,1,1);//because .mtl excluding specular , so give it here.
		for(int j = 0;j <mtls[i].size() ;j++){//
			mtlname = mtls[i][j];	
			//find the material diffuse color in map:KDs by material name.
			glUniform3fv(M_KdID,1,&KDs[mtlname][0]);
			glUniform3fv(M_KsID,1,&Ks[0]);
			glUniform1i(M_map_KdID, map_KDs[mtlname]);
			//          (primitive   , glVertexID base , vertex count    )
			glDrawArrays(GL_TRIANGLES, vertexIDoffset  , faces[i][j+1]*3);
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += faces[i][j+1]*3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	
		glUniform1i(colormodID, colormod);
		timer++;
		if (timer > 10000)
			timer=0;
		glUniform1f(timeID, timer);
		glUniform1i(colormodID, colormod);
	}//end for loop for updating and drawing model
	glFlush();//強制執行上次的OpenGL commands
	glutSwapBuffers();//調換前台和後台buffer ,當後臺buffer畫完和前台buffer交換使我們看見它
}

void Obj2Buffer(){
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::vector<std::string> Textures;
	loadMTL("Obj/body2.mtl", Kds, Kas, Kss, Materials, Textures);
	loadMTL("Obj/head2.mtl", Kds, Kas, Kss, Materials, Textures);
	loadMTL("Obj/lHand.mtl", Kds, Kas, Kss, Materials, Textures);
	loadMTL("Obj/rHand.mtl", Kds, Kas, Kss, Materials, Textures);
	loadMTL("Obj/lLeg.mtl", Kds, Kas, Kss, Materials, Textures);
	loadMTL("Obj/rLeg.mtl", Kds, Kas, Kss, Materials, Textures);
	//printf("%d\n",texture);
	for(int i = 0;i<Materials.size();i++){
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}
	for (int i = 0; i < Materials.size(); i++) {
		string mtlname = Materials[i];
		if (Textures[i] == "Body.png")
			map_KDs[mtlname] = 0;
		if (Textures[i] == "Eyes.png")
			map_KDs[mtlname] = 1;
		if (Textures[i] == "Zip.png")
			map_KDs[mtlname] = 2;
		if (Textures[i] == "tounge.png")
			map_KDs[mtlname] = 3;
	}
	
	/*load2Buffer("Obj/body.obj",0);

	load2Buffer("Obj/ulefthand.obj",1);
	load2Buffer("Obj/dlefthand.obj",2);
	load2Buffer("Obj/lHand.obj",3);
	load2Buffer("Obj/lshouder.obj",4);
	
	load2Buffer("Obj/head.obj",5);

	load2Buffer("Obj/urighthand.obj",6);
	load2Buffer("Obj/drighthand.obj",7);
	load2Buffer("Obj/righthand.obj",8);
	load2Buffer("Obj/rshouder.obj",9);

	load2Buffer("Obj/dbody.obj",11);
	load2Buffer("Obj/back2.obj",10);

	load2Buffer("Obj/uleftleg.obj",12);
	load2Buffer("Obj/dleftleg.obj",13);
	load2Buffer("Obj/leftfoot.obj",14);

	load2Buffer("Obj/urightleg.obj",15);	
	load2Buffer("Obj/drightleg.obj",16);	
	load2Buffer("Obj/rightfoot.obj",17);*/
	/*load2Buffer("Obj/body2.obj", 0);
	load2Buffer("Obj/head2.obj", 1);
	load2Buffer("Obj/luJoint.obj", 2);
	load2Buffer("Obj/lHand.obj", 6);
	load2Buffer("Obj/ruJoint.obj", 3);
	load2Buffer("Obj/rHand.obj", 7);
	load2Buffer("Obj/ldJoint.obj", 4);
	load2Buffer("Obj/lLeg.obj", 8);
	load2Buffer("Obj/rdJoint.obj", 5);
	load2Buffer("Obj/rLeg.obj", 9);*/

	load2Buffer("Obj/body2.obj", 0);
	load2Buffer("Obj/head2.obj", 1);
	load2Buffer("Obj/lHand.obj", 2);
	load2Buffer("Obj/rHand.obj", 3);
	load2Buffer("Obj/lLeg.obj", 4);
	load2Buffer("Obj/rLeg.obj", 5);
	
	GLuint totalSize[3] = {0,0,0};
	GLuint offset[3] = {0,0,0};
	for(int i = 0;i < PARTSNUM ;i++){
		totalSize[0] += vertices_size[i]*sizeof(vec3);
		totalSize[1] += uvs_size[i] * sizeof(vec2);
		totalSize[2] += normals_size[i] * sizeof(vec3);
	}
	//generate vbo
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&uVBO);
	glGenBuffers(1,&nVBO);
	//bind vbo ,第一次bind也同等於 create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[2],NULL,GL_STATIC_DRAW);
	
	
	for(int i = 0;i < PARTSNUM ;i++){
		glBindBuffer(GL_COPY_WRITE_BUFFER,VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[0],vertices_size[i]*sizeof(vec3));
		offset[0] += vertices_size[i]*sizeof(vec3);
		glInvalidateBufferData(VBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[1],uvs_size[i]*sizeof(vec2));
		offset[1] += uvs_size[i]*sizeof(vec2);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[2],normals_size[i]*sizeof(vec3));
		offset[2] += normals_size[i] * sizeof(vec3);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	}
	glBindBuffer(GL_COPY_WRITE_BUFFER,0);


}

void updateModels(){
	mat4 Rotatation[PARTSNUM];
	mat4 Translation[PARTSNUM];
	for(int i = 0 ; i < PARTSNUM;i++){
		Models[i] = mat4(1.0f);
		Rotatation[i] = mat4(1.0f);
		Translation[i] = mat4(1.0f); 
	}
	float r,pitch,yaw,roll;
	float alpha, beta ,gamma;

	//Body
	beta = angle;
	Rotatation[0] = rotate(beta,0,1,0);
	Translation[0] = translate(0,position,0);
	Models[0] = Translation[0]*Rotatation[0];
	//頭==========================================================
	Translation[1] = translate(0, 0, 0);
	Models[1] = Models[0] * Translation[1] * Rotatation[1];
	//============================================================
	//左手=======================================================
	//左手臂
	yaw = DOR(beta); r = 0;
	alpha = angles[2];
	gamma = 0;
	Rotatation[2] = rotate(alpha, 1, 0, 0)*rotate(gamma, 0, 0, 1);//向前旋轉*向右旋轉
	Translation[2] = translate(0, 0, 0);

	Models[2] = Models[0] * Translation[2] * Rotatation[2];
	//============================================================
	//右手=========================================================
	gamma = 0; alpha = angles[3] = -angles[2];
	Rotatation[3] = rotate(alpha, 1, 0, 0)*rotate(gamma, 0, 0, 1);
	Translation[3] = translate(0, 0, 0);
	Models[3] = Models[0] * Translation[3] * Rotatation[3];
	//=============================================================
	//左腳
	alpha = angles[4]; gamma = 0;
	Rotatation[4] = rotate(alpha, 1, 0, 0)*rotate(gamma, 0, 0, 1);
	Translation[4] = translate(0, 0, 0);
	Models[4] = Translation[4] * Rotatation[4] * Models[4];
	//=============================================================
	//右腳
	alpha = angles[5] = -angles[4];
	gamma = 0;
	Rotatation[5] = rotate(alpha, 1, 0, 0)*rotate(gamma, 0, 0, 1);
	Translation[5] = translate(0, 0, 0);
	Models[5] = Translation[5] * Rotatation[5] * Models[5];
	//=============================================================
	/*//左手=======================================================
	//左上手臂
	yaw = DOR(beta);r = 3.7;
	alpha = angles[1];
	gamma = 10;
	Rotatation[1] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	Translation[1] = translate(3.7,1,-0.5);

	Models[1] = Models[0]*Translation[1]*Rotatation[1];
	
	//左肩膀
	Rotatation[4] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	Translation[4] =translate(3.7,1,-0.5);//位移到左上手臂處
	Models[4] =Models[0]*Translation[1]*Rotatation[1];
	
	//左下手臂
	pitch = DOR(alpha);r = 3;
	roll = DOR(gamma);
	static int i=0;
	i+=5;
	alpha = angles[2]-20;
	//上手臂+下手臂向前旋轉*向右旋轉
	Rotatation[2] = rotate(alpha,1,0,0);
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin)
	//延z軸位移以上手臂為半徑角度:translate(r*sin,-rcos,0)
	Translation[2] = translate(0,-3,0);

	Models[2] = Models[1]*Translation[2]*Rotatation[2];
	

	pitch = DOR(alpha);
	//b = DOR(angles[2]);
	roll = DOR(gamma);
	//手掌角度與下手臂相同
	//Rotatation[3] = Rotatation[2];
	//延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin) ,角度為上手臂+下手臂
	Translation[3] = translate(0,-4.8,0);
	Models[3] = Models[2]*Translation[3]*Rotatation[3];
	//============================================================
	//頭==========================================================
	Translation[5] = translate(0,3.9,-0.5);
	Models[5] = Models[0]*Translation[5]*Rotatation[5];
	//============================================================
	//右手=========================================================
	gamma = -10;alpha = angles[6] = -angles[1];
	Rotatation[6] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	Translation[6] = translate(-3.9,1.7,-0.2);
	Models[6] = Models[0]*Translation[6]*Rotatation[6];

	Rotatation[9] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	Translation[9] = translate(-3.9,1.1,-0.2);
	Models[9] = Models[0]*Translation[9]*Rotatation[9];

	angles[7] = angles[2];
	pitch = DOR(alpha);r = -3;
	roll = DOR(gamma);
	alpha = angles[7]-20;
	Rotatation[7] = rotate(alpha,1,0,0);
	Translation[7] = translate(0,-3,0);
	Models[7] = Models[6]*Translation[7]*Rotatation[7];

	pitch = DOR(alpha);
	//b = DOR(angles[7]);
	roll = DOR(gamma);
	Translation[8] =translate(0,-6,0);
	Models[8] = Models[7]*Translation[8]*Rotatation[8];
	//=============================================================
	//back&DBody===================================================
	Translation[10] =translate(0,2,-4.5);
	Models[10] = Models[0]*Translation[10]*Rotatation[10];

	Translation[11] =translate(0,-5.3,0);
	Models[11] = Models[0]*Translation[11]*Rotatation[11];
	//=============================================================
	//左腳
	alpha = angles[12];gamma = 10;
	Rotatation[12] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	Translation[12] =translate(1.8,-4.5,0);
	Models[12] = Translation[12]*Rotatation[12]*Models[12];

	pitch = DOR(alpha);r = -7;
	roll = DOR(gamma);
	alpha = angles[13]+angles[12];
	Translation[13] = translate(-r*sin(roll),r*cos(pitch),r*sin(pitch))*Translation[12];
	Rotatation[13] = rotate(alpha,1,0,0);
	Models[13] = Translation[13]*Rotatation[13]*Models[13];

	pitch = DOR(alpha); r = -5;
	//b = DOR(angles[13]);
	roll = DOR(gamma);
	Translation[14] = translate(-(r+2)*sin(roll),r*cos(pitch),r*sin(pitch)-1)*Translation[13];
	Rotatation[14] = rotate(alpha,1,0,0);
	Models[14] = Translation[14]*Rotatation[14]*Models[14];
	//=============================================================
	//右腳
	alpha = angles[15] = -angles[12];
	gamma = -10;
	Rotatation[15] = rotate(alpha ,1,0,0)*rotate(gamma ,0,0,1);
	Translation[15] =translate(-1.8,-4.5,0);
	Models[15] = Translation[15]*Rotatation[15]*Models[15];

	angles[16] = angles[13];
	pitch = DOR(alpha);r = -7;
	roll = DOR(gamma);
	alpha = angles[16]+angles[15];
	Rotatation[16] = rotate(alpha,1,0,0);
	Translation[16] = translate(-r*sin(roll),r*cos(pitch),r*sin(pitch))*Translation[15];
	Models[16] = Translation[16]*Rotatation[16]*Models[16];

	pitch = DOR(alpha); r = -5;
	//b = DOR(angles[16]);
	roll = DOR(gamma);
	alpha = angles[15]+angles[16];
	Translation[17] = translate(-(r+2)*sin(roll),r*cos(pitch),r*sin(pitch)-0.5)*Translation[16];
	Rotatation[17] = rotate(alpha,1,0,0);
	Models[17] = Translation[17]*Rotatation[17]*Models[17];
	//=============================================================*/
}

void load2Buffer(char* obj,int i){
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals; // Won't be used at the moment.
	std::vector<unsigned int> materialIndices;

	bool res = loadOBJ(obj, vertices, uvs, normals,faces[i],mtls[i]);
	if(!res) printf("load failed\n");

	//glUseProgram(program);

	glGenBuffers(1,&VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),&vertices[0],GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	//(buffer type,data起始位置,data size,data first ptr)
	//vertices_size[i] = glm_model->numtriangles;
	
	//printf("vertices:%d\n",vertices_size[);

	glGenBuffers(1,&uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1,&nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}
mat4 translate(float x,float y,float z){
	vec4 t = vec4(x,y,z,1);//w = 1 ,則x,y,z=0時也能translate
	vec4 c1 = vec4(1,0,0,0);
	vec4 c2 = vec4(0,1,0,0);
	vec4 c3 = vec4(0,0,1,0);
	mat4 M = mat4(c1,c2,c3,t);
	return M;
} 
mat4 scale(float x,float y,float z){
	vec4 c1 = vec4(x,0,0,0);
	vec4 c2 = vec4(0,y,0,0);
	vec4 c3 = vec4(0,0,z,0);
	vec4 c4 = vec4(0,0,0,1);
	mat4 M = mat4(c1,c2,c3,c4);
	return M;
}

mat4 rotate(float angle,float x,float y,float z){
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r)+(1-cos(r))*x*x,(1-cos(r))*y*x+sin(r)*z,(1-cos(r))*z*x-sin(r)*y,0);
	vec4 c2 = vec4((1-cos(r))*y*x-sin(r)*z,cos(r)+(1-cos(r))*y*y,(1-cos(r))*z*y+sin(r)*x,0);
	vec4 c3 = vec4((1-cos(r))*z*x+sin(r)*y,(1-cos(r))*z*y-sin(r)*x,cos(r)+(1-cos(r))*z*z,0);
	vec4 c4 = vec4(0,0,0,1);
	M = mat4(c1,c2,c3,c4);
	return M;
}
void Keyboard(unsigned char key, int x, int y){
	switch(key){
	case '1':
		angle += 5;
		if(angle>=360) angle = 0;
		printf("beta:%f\n",angle);
		break;
	case '2':
		angle -= 5;
		if(angle<=0) angle = 360;
		printf("beta:%f\n",angle);
		break;
	case 'w':
		eyedistance -= 0.2;
		break;
	case 's':
		eyedistance += 0.2;
		break;
	case 'a':
		eyeAngley -=10;
		break;
	case 'd':
		eyeAngley +=10;
		break;
	case 'r':
		angles[1] -= 5; 
		if(angles[1] == -360) angles[1] = 0;
		movey = 0;
		movex = 0;
		break;
	case 't':
		angles[2] -= 5;
		if(angles[2] == -360) angles[2] = 0;
		movey = 0;
		movex = 0;
		break;
	case 'q':
		break;
	case 'e':
		break;
	}
	glutPostRedisplay();
}
void menuEvents(int option){}
void ActionMenuEvents(int option){
	switch(option){
	case 0:
		action = IDLE;
		break;
	case 1:
		action = WALK;
		break;
	}
}
void ColorMenuEvents(int option) {
	switch (option) {
	case 0:
		colormod = 0;
		break;
	case 1:
		colormod = 1;
		break;
	case 2:
		colormod = 2;
		break;
	case 3:
		colormod = 3;
		break;
	case 4:
		colormod = 4;
		break;
	}
}
void ModeMenuEvents(int option){
	switch(option){
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}
void ShaderMenuEvents(int option){
	pNo = option;
}
