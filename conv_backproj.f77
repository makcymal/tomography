      program backprojection

      real grid(-80:80, 200), conv(-80:80, 200), max, f, x, y
      integer pts, dirs, nx, ny


      open(1, file='pts_dirs.dat')
      read(1, *) pts, dirs
      close(1)


      open(2, file='grid.dat')
      do 10 dir = 1, dirs
        do 10 pt = -pts, pts
          read(2, *) grid(pt, dir)
 10   continue
      close(2)


      call convolution(grid, conv, pts, dirs)


      open(3, file='xy.dat')
      read(3, *) nx, ny
      close(3)


      f = 0.0
      max = 0.0
      open(4, file='f.dat')
      
      do 20 i = -nx, nx
        y = -1.0 * i / nx
        do 20 j = -ny, ny
          x = 1.0 * j / ny
  
          if (x*x + y*y .ge. 1) f = 0.0
          else call backprojection(f, x, y, conv, pts, dirs)
          endif
  
          if (f .le. 0) f = 0.0
          elseif (f .ge. max) max = f
          endif
  
          write(4, *) f
 20     continue

      close(4)


      open(5, file='x_y.dat')
      write(5, *) 2*nx + 1, 2*ny + 1
      write(5, *) -1.0, 1.0 / nx
      write(5, *) 1.0, 1.0 / ny
      write(5, *) max

      stop
      end



      subroutine convolution(grid, conv, pts, dirs)

      real grid(-80:80, 200), conv(-80:80, 200), sum
      integer pts, dirs

*     Для каждой точки pt из [-pts, pts], каждого направления dir из [1, dirs]
*     вычисляются свертки conv(pt, dir)

      do 20 pt = -pts, pts
        do 20 dir = 1, dirs
          sum = 0.0
          do 10 pti = -pts, pts
            sum = sum + grid(pti, dir) / (1.0 - 4.0 * (pt - pti) * (pt - pti))
 10       continue
          conv(pt, dir) = sum
 20     continue

      return
      end




      subroutine backprojection(f, x, y, conv, pts, dirs)

      real f, x, y, conv(-80:80, 200), sum, phi, pi, s, fs, u, 
      integer pts, dirs, is
      pi = 3.14159263

*     В каждой точке восстановления вычисляется дискретная обратная проекция

      sum = 0.0
      do 10 dir = 1, dirs
*       e.g. phi = 0, 0.1pi, 0.2pi, ..., 0.9pi if dirs = 10
        phi = pi * (dir - 1.0) / dirs
        s = x * cos(phi) + y * sin(phi)
        fs = s * pts
        is = s * pts
        u = abs(fs - is)
        sum = sum + (1.0 - u) * conv(is, dir) + u * conv(is + 1, dir)
 10   continue

      f = 2.0 * pts * sum / (pi * dirs)

      return
      end