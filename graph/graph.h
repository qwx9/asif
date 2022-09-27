typedef struct Vertex Vertex;
typedef struct Vectangle Vectangle;
struct Vertex{
	int x;
	int y;
};
struct Vectangle{
	Vertex min;
	Vertex max;
};

Vertex	V(int, int);
Vectangle	V²(int, int, int, int);
Vectangle	toV²(Vertex, Vertex);
Vertex	ΔV(Vertex, Vertex);
Vertex	∑V(Vertex, Vertex);
int	V∩V²(Vertex, Vectangle);
