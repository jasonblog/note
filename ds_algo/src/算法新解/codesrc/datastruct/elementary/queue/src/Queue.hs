{-
    Queue.hs, General type definition for Queue (FIFO)
    Copyright (C) 2012, Liu Xinyu (liuxinyu95@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-}

-- Contains both general type definition and test utility.

module Queue where

-- Definition
class Queue q where
    empty :: q a
    isEmpty :: q a -> Bool
    push :: q a -> a -> q a -- aka 'snoc' or append, or push_back
    pop :: q a -> q a -- aka 'tail' or pop_front
    front :: q a -> a -- aka 'head'

-- test

proc :: (Queue q)=>[Int] -> q Int -> [Int]
proc [] _ = []
proc (x:xs) q | even x = proc xs (push q x)
              | isEmpty q = proc xs q
              | otherwise = front q : proc xs (pop q)

proc' :: [Int] -> [Int] -> [Int]
proc' [] _ = []
proc' (x:xs) lst | even x = proc' xs (lst ++ [x])
                 | null lst = proc' xs lst
                 | otherwise = head lst : proc' xs (tail lst)

