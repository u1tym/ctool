abc :: Int -> Int -> Int
abc x y | x > 0     = x + y
abc x y | otherwise = p
    where p = ( fnc x ) * y

