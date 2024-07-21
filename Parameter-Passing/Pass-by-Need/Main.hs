module Main where

import System.CPUTime
import Text.Printf

main :: IO ()
main = do
    start <- getCPUTime
    let result = sum (take 1000000 [1..])
    mid <- getCPUTime
    putStrLn $ "The sum is: " ++ show result
    end <- getCPUTime
    
    let diff1 = fromIntegral (mid - start) / (10^12) :: Double
    let diff2 = fromIntegral (end - mid) / (10^12) :: Double
    
    printf "Time to create the list and sum: %0.8f seconds\n" diff1
    printf "Time to print the result: %0.8f seconds\n" diff2
