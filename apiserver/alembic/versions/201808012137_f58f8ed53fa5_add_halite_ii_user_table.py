"""Add Halite II user table

Revision ID: f58f8ed53fa5
Revises: 02dea09f298a
Create Date: 2018-08-01 21:37:21.026493+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = 'f58f8ed53fa5'
down_revision = '02dea09f298a'
branch_labels = None
depends_on = None


def upgrade():
    op.create_table('halite_2_user',
                    sa.Column('id', sa.Integer(), nullable=False),
                    sa.Column('oauth_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.Column('username', sa.Unicode(length=40), nullable=False),
                    sa.Column('email', sa.Unicode(length=320), nullable=True),
                    sa.Column('github_email', sa.Unicode(length=320), nullable=True),
                    sa.Column('player_level', sa.Enum('High School', 'University', 'Professional', name='halite_2_user_player_level'), server_default=sa.text("'Professional'"), nullable=False),
                    sa.Column('organization_name', sa.Unicode(), nullable=True),
                    sa.Column('creation_time', sa.TIMESTAMP(timezone=True), nullable=False),
                    sa.Column('update_time', sa.TIMESTAMP(timezone=True), nullable=True),
                    sa.Column('language', sa.Unicode(length=64), nullable=True),
                    sa.Column('version_number', sa.Integer(), autoincrement=False, nullable=True),
                    sa.Column('games_played', sa.Integer(), autoincrement=False, nullable=True),
                    sa.Column('mu', sa.Float(), nullable=True),
                    sa.Column('sigma', sa.Float(), nullable=True),
                    sa.Column('score', sa.Float(), nullable=True),
                    sa.PrimaryKeyConstraint('id'),
                    )


def downgrade():
    op.drop_table('halite_2_user')
    sa.Enum(name='halite_2_user_player_level').drop(op.get_bind())
