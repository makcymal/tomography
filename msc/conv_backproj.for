      program backprojection

      real grid(-80:80, 200), conv(-80:80, 200), max, f, x, y
      integer pts, dirs, nx, ny


      open(1, file='pts_dirs.dat')
      read(1, *) pts, dirs
      close(1)


      open(2, file='grid.dat')
      do dir = 1, dirs
        do pt = -pts, pts
          read(2, *) grid(pt, dir)
        end do
      end do
      close(2)


      call convolution(grid, conv, pts, dirs)


      open(3, file='xy.dat')
      read(3, *) nx, ny
      close(3)


      f = 0.0
      max = 0.0
      open(4, file='f.dat')
      
      do i = -nx, nx
        y = -1.0 * i / nx
        do j = -ny, ny
          x = 1.0 * j / ny

          if (x*x + y*y >= 1) then
            f = 0.0
          else
            call backprojection(f, x, y, conv, pts, dirs)
          endif

          if (f <= 0) then
            f = 0.0
          elseif (f >= max) then
            max = f
          endif

          write(4, *) f
        end do
      end do

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

*     Для каждой точки pt из [-pts, pts], каждого направления dir из [1, dirs] вычисляются свертки conv(pt, dir)

      do pt = -pts, pts
        do dir = 1, dirs
          sum = 0.0
          do pti = -pts, pts
            sum = sum + grid(pti, dir) / (1.0 - 4.0 * (pt - pti) * (pt - pti))
          end do
          conv(pt, dir) = sum
        end do
      end do

      return
      end



      subroutine backprojection(f, x, y, conv, pts, dirs)

      real f, x, y, conv(-80:80, 200), sum, phi, pi, rotsin, rs, s_mantissa
      integer pts, dirs, is
      pi = 3.14159263

*     В каждой точке восстановления вычисляется дискретная обратная проекция

      sum = 0.0
      do dir = 1, dirs
*       e.g. phi = 0, 0.1pi, 0.2pi, ..., 0.9pi if dirs = 10
        phi = pi * (dir - 1.0) / dirs
        rotsin = x * cos(phi) + y * sin(phi)
        rs = rotsin * pts
        is = rotsin * pts
        s_mantissa = abs(rs - is)
        sum = sum + (1.0 - s_mantissa) * conv(is, dir) + s_mantissa * conv(is + 1, dir)
      end do

      f = 2.0 * pts * sum / (pi * dirs)

      return
      end