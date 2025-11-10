# **Manual**

### **Uso del parser OBJ (cómo lo estoy usando)**

Tengo una clase llamada ObjReader que uso para cargar modelos .obj y dejarlos listos en un MeshComponent, que luego va a los buffers de DirectX.

La función principal es:

bool ObjReader::load(const std::string& path, MeshComponent& outMesh, bool flipV);

Yo la uso más o menos así:

ObjReader    m\_modelLoader;

MeshComponent m\_mesh;

if (\!m\_modelLoader.load("Alien", m\_mesh)) {

    // aquí manejo el error (mensaje, return, etc.)

}

* Si paso "Alien", por dentro el parser le agrega .obj si no lo tiene.

* También se puede llamar con ruta completa, tipo "models/Alien.obj".

El parámetro flipV sirve por si las UV salen volteadas en vertical.  
Por defecto es true, entonces hace v \= 1.0f \- v al leer las vt.  
Si la textura se ve bien sin voltear, se puede llamar así:

m\_modelLoader.load("Alien", m\_mesh, false);

### **Qué deja listo el parser en la malla**

Después de llamar a load, el MeshComponent queda con:

m\_mesh.m\_vertex      // vector\<SimpleVertex\> (posición \+ uv)

m\_mesh.m\_index       // vector\<unsigned int\> (índices)

m\_mesh.m\_numVertex   // número de vértices

m\_mesh.m\_numIndex    // número de índices

El SimpleVertex que uso es:

struct SimpleVertex {

    XMFLOAT3 Pos;

    XMFLOAT2 Tex;

};

Con esos datos ya creo los buffers de GPU:

m\_vertexBuffer.init(m\_device, m\_mesh, D3D11\_BIND\_VERTEX\_BUFFER);

m\_indexBuffer.init(m\_device, m\_mesh, D3D11\_BIND\_INDEX\_BUFFER);

Y en el render hago el DrawIndexed con m\_mesh.m\_numIndex.

### **Qué cosas del OBJ estoy leyendo**

De todo el formato OBJ, yo solo estoy usando lo básico:

* v x y z → posiciones de vértice.

* vt u v → coordenadas de textura.

* vn nx ny nz → las leo pero de momento casi no las uso.

* f ... → caras, que pueden venir como:

  * v/vt/vn

  * v/vt

  * v//vn

  * v

Las caras que tienen más de 3 vértices las convierto a triángulos con un “fan”:

* si la cara tiene vértices 0,1,2,3, genero triángulos:

  * (0,1,2) y (0,2,3)

El resto de cosas del OBJ (g, o, usemtl, etc.) las ignoro por ahora porque para este proyecto solo necesito geometría \+ UV para poder texturizar el modelo.

