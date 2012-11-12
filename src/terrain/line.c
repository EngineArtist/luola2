inline bool lines_intersect_2d(Vector2 const& p0, Vector2 const& p1, Vector2 const& p2, Vector2 const& p3, Vector2* i const = 0) {
    Vector2 const s1 = p1 - p0;
    Vector2 const s2 = p3 - p2;

    Vector2 const u = p0 - p2;

    float const ip = 1.f / (-s2.x * s1.y + s1.x * s2.y);

    float const s = (-s1.y * u.x + s1.x * u.y) * ip;
    float const t = ( s2.x * u.y - s2.y * u.x) * ip;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        if (i) *i = p0 + (s1 * t);
        return true;
    }

    return false;
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// // intersect the intersection point may be stored in the floats i_x and i_y.
//
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

