<!-- Formatting Docs -->
<!-- https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax -->
# *Mobile Game (solo project)*

The assignment was simple use Unity, make a mobile game, use some programming patterns, go.


I have always quite liked Subway Surfer and obviously looked quite close on that game during the design of my project. As I saw this as a learning opportunity i focused on getting some kind of base of that game working in 3D with the core mechanics and a similar feel to that game in responsiveness and player control.

I like the simple design, and the quite good mechanics with jump canceling, ducking, powerups, and the flow of their levels. It's semi predictable chunks of level sections that you can learn to get good at the game. And it felt like a reasonable challenge at my stage of development.


I got a basic prototype up and running in quite a short time.
And then refined it in many different iterations.

Thinking trough the level generation and what I wanted out of it was a challenge. As ideally it should be playable during all the different speeds that the game throws at the player.
In the beginning I used the most simple approach just having a bunch of level chunks that get picked at random.
Then I looked into Scriptable Objects and made the level data an Scriptable Object,
to keep the references of the data inside. 
And a another scriptable object containing an array and information needed to spawn the array in order or spawn them randomly.
That allowed me to design level sections that could be somewhat random but have a controllable flow. Allowing for rules regarding spawning.


The art is quite close to subway surfer in its current stage, as it was easy to model out and make the first version of. The plan was always to redesign it to be a dungeon theme, using different monsters as obstacles that you slice trough using a greatsword. I made a simple sword swinging (red cube) click action that you can use on smaller obstacles.
But sadly the time alloted did not allowed me to make those changes to art. And there were quit a lot of non project time going to reading about patterns and making small side projects to learn more about them as well.

The hardest part was setting upp all the systems and getting all the right things talking to each other. I rebuilt them 1, or 2 times but there is still some things that irk me.
And maybe it would have been better letting the player just run and reset the world and move everything back to origin during one frame after some distance, like many similar games do to keep some things more straight forward.

In hindsight i would have done some things different, and refactored quite a lot. But as per usual in game development you do what you can to get something working and out the door, to the best of your available time and skills.

Im still pleased with the project overall and happy that I made it. I learnt a lot of things and see fun in it even if I would like to update the art and give the levels some more variation in patterns.