Coordinate in camera coordinate system: (X, Y, Z)
Coordinate in camera image plane: (u, v, f)
    The image plane is at z = f from the origin of camera coordinate system.
    f is called the camera focal length.
    u = X * (f/Z)
    v = Y * (f/Z)
    rho = f/Z : distance normalized by focal length
    u' = X * (f/Z) + Pu
    v' = Y * (f/Z) + Pv
    (Pu, Pv): Principal point of camera image plane
    x = Mu * {X * (f/Z) + Pu}
    y = Mv * {Y * (f/Z) + Pv}
    (x, y): pixel coordinate
    x = Ax * X / Z + Px
    y = Ay * Y / Z + Py
    Z * x = Ax * X + Px * Z
    Z * y = Ay * Y + Py * Z
    Z [x, y, 1]t = [[Ax, 0, Px], [0, Ay, Py], [0, 0, 1]] [X, Y, Z]
    Introducing a skew factor S
    Z [x, y, 1]t = [[Ax, S, Px], [0, Ay, Py], [0, 0, 1]] [X, Y, Z]
    Camera intrinsic matrix K
    K = [[Ax, S, Px], [0, Ay, Py], [0, 0, 1]]



Calibration point set
Calibration board
    (i * C, j * C, 0): vertical, horizontal pitch = C, (i, j: section point index, ..., -2, -1, 0, 1, 2, ...)
    Usually section point indices are typically in a range of [-3, +3]
    At the 1st location : (X0, Y0, Z0)
    At the 2nd location : (X0 + dX, Y0 + dY, Z0 + dZ), norm2(dX,dY,dZ) = D^2